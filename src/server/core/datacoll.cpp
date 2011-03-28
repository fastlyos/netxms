/* 
** NetXMS - Network Management System
** Copyright (C) 2003-2010 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: datacoll.cpp
**
**/

#include "nxcore.h"


//
// Constants
//

#define ITEM_POLLING_INTERVAL    2


//
// Externals
//

extern Queue g_statusPollQueue;
extern Queue g_configPollQueue;


//
// Global data
//

double g_dAvgPollerQueueSize = 0;
double g_dAvgDBWriterQueueSize = 0;
double g_dAvgStatusPollerQueueSize = 0;
double g_dAvgConfigPollerQueueSize = 0;
DWORD g_dwAvgDCIQueuingTime = 0;
Queue *g_pItemQueue = NULL;


//
// Data collector
//

static THREAD_RESULT THREAD_CALL DataCollector(void *pArg)
{
   DCItem *pItem;
   Node *pNode;
   DWORD dwError;
   time_t currTime;
   TCHAR *pBuffer;

   pBuffer = (TCHAR *)malloc(MAX_LINE_SIZE * sizeof(TCHAR));

   while(!ShutdownInProgress())
   {
      pItem = (DCItem *)g_pItemQueue->GetOrBlock();
		pNode = (Node *)pItem->getRelatedNode();

		if (pItem->isScheduledForDeletion())
		{
	      DbgPrintf(7, _T("DataCollector(): about to destroy DCI %d \"%s\" node=%d"),
			          pItem->getId(), pItem->getName(), (pNode != NULL) ? pNode->Id() : -1);
			pItem->deleteFromDB();
			delete pItem;
			continue;
		}

      DbgPrintf(8, _T("DataCollector(): processing DCI %d \"%s\" node=%d proxy=%d"),
		          pItem->getId(), pItem->getName(), (pNode != NULL) ? pNode->Id() : -1, pItem->getProxyNode());
		if (pItem->getProxyNode() != 0)
		{
			NetObj *object;

			object = FindObjectById(pItem->getProxyNode());
			if (object != NULL)
			{
				if ((object->Type() == OBJECT_NODE) &&
					(object->IsTrustedNode((pNode != NULL) ? pNode->Id() : 0)))
				{
					pNode = (Node *)object;
					pNode->IncRefCount();
				}
				else
				{
               // Change item's status to _T("not supported")
               pItem->setStatus(ITEM_STATUS_NOT_SUPPORTED, true);

					if (pNode != NULL)
					{
						pNode->DecRefCount();
						pNode = NULL;
					}
				}
			}
			else
			{
				if (pNode != NULL)
				{
					pNode->DecRefCount();
					pNode = NULL;
				}
			}
		}

      if (pNode != NULL)
      {
         switch(pItem->getDataSource())
         {
            case DS_INTERNAL:    // Server internal parameters (like status)
               dwError = pNode->GetInternalItem(pItem->getName(), MAX_LINE_SIZE, pBuffer);
               break;
            case DS_SNMP_AGENT:
					dwError = pNode->GetItemFromSNMP(pItem->getSnmpPort(), pItem->getName(), MAX_LINE_SIZE, pBuffer);
               break;
            case DS_CHECKPOINT_AGENT:
               dwError = pNode->GetItemFromCheckPointSNMP(pItem->getName(), MAX_LINE_SIZE, pBuffer);
               break;
            case DS_NATIVE_AGENT:
               dwError = pNode->GetItemFromAgent(pItem->getName(), MAX_LINE_SIZE, pBuffer);
               break;
         }

         // Get polling time
         currTime = time(NULL);

         // Transform and store received value into database or handle error
         switch(dwError)
         {
            case DCE_SUCCESS:
					if (pItem->getStatus() == ITEM_STATUS_NOT_SUPPORTED)
	               pItem->setStatus(ITEM_STATUS_ACTIVE, true);
					((Node *)pItem->getRelatedNode())->processNewDciValue(pItem, currTime, pBuffer);
               break;
            case DCE_COMM_ERROR:
               pItem->processNewError();
               break;
            case DCE_NOT_SUPPORTED:
               // Change item's status
               pItem->setStatus(ITEM_STATUS_NOT_SUPPORTED, true);
               break;
         }

         // Decrement node's usage counter
         pNode->DecRefCount();
			if ((pItem->getProxyNode() != 0) && (pItem->getRelatedNode() != NULL))
			{
				pItem->getRelatedNode()->DecRefCount();
			}
      }
      else     /* pNode == NULL */
      {
			Template *n = pItem->getRelatedNode();
         DbgPrintf(3, _T("*** DataCollector: Attempt to collect information for non-existing node (DCI=%d \"%s\" node=%d proxy=%d)"),
			          pItem->getId(), pItem->getName(), (n != NULL) ? n->Id() : -1, pItem->getProxyNode());
      }

		// Update item's last poll time and clear busy flag so item can be polled again
      pItem->setLastPollTime(currTime);
      pItem->setBusyFlag(FALSE);
   }

   free(pBuffer);
   DbgPrintf(1, _T("Data collector thread terminated"));
   return THREAD_OK;
}


//
// Item poller thread: check nodes' items and put into the 
// data collector queue when data polling required
//

static THREAD_RESULT THREAD_CALL ItemPoller(void *pArg)
{
   DWORD i, dwSum, dwWatchdogId, dwCurrPos = 0;
   DWORD dwTimingHistory[60 / ITEM_POLLING_INTERVAL];
   INT64 qwStart;

   dwWatchdogId = WatchdogAddThread(_T("Item Poller"), 20);
   memset(dwTimingHistory, 0, sizeof(DWORD) * (60 / ITEM_POLLING_INTERVAL));

   while(!ShutdownInProgress())
   {
      if (SleepAndCheckForShutdown(ITEM_POLLING_INTERVAL))
         break;      // Shutdown has arrived
      WatchdogNotify(dwWatchdogId);
		DbgPrintf(8, _T("ItemPoller: wakeup"));

      RWLockReadLock(g_rwlockIdIndex, INFINITE);
		DbgPrintf(8, _T("ItemPoller: object index lock acquired (index size %d)"), g_dwIdIndexSize);
      qwStart = GetCurrentTimeMs();
      for(i = 0; i < g_dwIdIndexSize; i++)
		{
			if (((NetObj *)g_pIndexById[i].pObject)->Type() == OBJECT_NODE)
			{
				DbgPrintf(8, _T("ItemPoller: (%d) calling QueueItemsForPolling for node %s [%d]"),
							 i, ((Node *)g_pIndexById[i].pObject)->Name(), ((Node *)g_pIndexById[i].pObject)->Id());
				((Node *)g_pIndexById[i].pObject)->queueItemsForPolling(g_pItemQueue);
			}
		}
      RWLockUnlock(g_rwlockIdIndex);
		DbgPrintf(8, _T("ItemPoller: object index lock released"));

      // Save last poll time
      dwTimingHistory[dwCurrPos] = (DWORD)(GetCurrentTimeMs() - qwStart);
      dwCurrPos++;
      if (dwCurrPos == (60 / ITEM_POLLING_INTERVAL))
         dwCurrPos = 0;

      // Calculate new average for last minute
      for(i = 0, dwSum = 0; i < (60 / ITEM_POLLING_INTERVAL); i++)
         dwSum += dwTimingHistory[i];
      g_dwAvgDCIQueuingTime = dwSum / (60 / ITEM_POLLING_INTERVAL);
   }
   DbgPrintf(1, _T("Item poller thread terminated"));
   return THREAD_OK;
}


//
// Statistics collection thread
//

static THREAD_RESULT THREAD_CALL StatCollector(void *pArg)
{
   DWORD i, dwCurrPos = 0;
   DWORD dwPollerQS[12], dwDBWriterQS[12];
   DWORD dwStatusPollerQS[12], dwConfigPollerQS[12];
   double dSum1, dSum2, dSum3, dSum4;

   memset(dwPollerQS, 0, sizeof(DWORD) * 12);
   memset(dwDBWriterQS, 0, sizeof(DWORD) * 12);
   memset(dwStatusPollerQS, 0, sizeof(DWORD) * 12);
   memset(dwConfigPollerQS, 0, sizeof(DWORD) * 12);
   g_dAvgPollerQueueSize = 0;
   g_dAvgDBWriterQueueSize = 0;
   g_dAvgStatusPollerQueueSize = 0;
   g_dAvgConfigPollerQueueSize = 0;
   while(!ShutdownInProgress())
   {
      if (SleepAndCheckForShutdown(5))
         break;      // Shutdown has arrived

      // Get current values
      dwPollerQS[dwCurrPos] = g_pItemQueue->Size();
      dwDBWriterQS[dwCurrPos] = g_pLazyRequestQueue->Size();
      dwStatusPollerQS[dwCurrPos] = g_statusPollQueue.Size();
      dwConfigPollerQS[dwCurrPos] = g_configPollQueue.Size();
      dwCurrPos++;
      if (dwCurrPos == 12)
         dwCurrPos = 0;

      // Calculate new averages
      for(i = 0, dSum1 = 0, dSum2 = 0, dSum3 = 0, dSum4 = 0; i < 12; i++)
      {
         dSum1 += dwPollerQS[i];
         dSum2 += dwDBWriterQS[i];
         dSum3 += dwStatusPollerQS[i];
         dSum4 += dwConfigPollerQS[i];
      }
      g_dAvgPollerQueueSize = dSum1 / 12;
      g_dAvgDBWriterQueueSize = dSum2 / 12;
      g_dAvgStatusPollerQueueSize = dSum3 / 12;
      g_dAvgConfigPollerQueueSize = dSum4 / 12;
   }
   return THREAD_OK;
}


//
// Initialize data collection subsystem
//

BOOL InitDataCollector()
{
   int i, iNumCollectors;

   // Create collection requests queue
   g_pItemQueue = new Queue(4096, 256);

   // Start data collection threads
   iNumCollectors = ConfigReadInt(_T("NumberOfDataCollectors"), 10);
   for(i = 0; i < iNumCollectors; i++)
      ThreadCreate(DataCollector, 0, NULL);

   // Start item poller thread
   ThreadCreate(ItemPoller, 0, NULL);

   // Start statistics collection thread
   ThreadCreate(StatCollector, 0, NULL);

   return TRUE;
}


//
// Write full list of supported parameters (from all nodes) to message
//

void WriteFullParamListToMessage(CSCPMessage *pMsg)
{
   DWORD i, j, k, dwNumParams, dwFullListSize;
   NXC_AGENT_PARAM *pParamList, *pFullList;

   // Gather full parameter list
   RWLockReadLock(g_rwlockIdIndex, INFINITE);
   for(i = 0, dwFullListSize = 0, pFullList = NULL; i < g_dwIdIndexSize; i++)
   {
		if (((NetObj *)g_pIndexById[i].pObject)->Type() == OBJECT_NODE)
		{
			((Node *)g_pIndexById[i].pObject)->OpenParamList(&dwNumParams, &pParamList);
			if ((dwNumParams > 0) && (pParamList != NULL))
			{
				pFullList = (NXC_AGENT_PARAM *)realloc(pFullList, sizeof(NXC_AGENT_PARAM) * (dwFullListSize + dwNumParams));
				for(j = 0; j < dwNumParams; j++)
				{
					for(k = 0; k < dwFullListSize; k++)
					{
						if (!_tcsicmp(pFullList[k].szName, pParamList[j].szName))
							break;
					}
					if (k == dwFullListSize)
					{
						memcpy(&pFullList[k], &pParamList[j], sizeof(NXC_AGENT_PARAM));
						dwFullListSize++;
					}
				}
			}
			((Node *)g_pIndexById[i].pObject)->CloseParamList();
		}
   }
   RWLockUnlock(g_rwlockIdIndex);

   // Put list into the message
   pMsg->SetVariable(VID_NUM_PARAMETERS, dwFullListSize);
   for(i = 0, j = VID_PARAM_LIST_BASE; i < dwFullListSize; i++)
   {
      pMsg->SetVariable(j++, pFullList[i].szName);
      pMsg->SetVariable(j++, pFullList[i].szDescription);
      pMsg->SetVariable(j++, (WORD)pFullList[i].iDataType);
   }

   safe_free(pFullList);
}
