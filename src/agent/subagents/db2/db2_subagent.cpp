/**
 * NetXMS - open source network management system
 * Copyright (C) 2013 Raden Solutions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "db2_subagent.h"

DB_DRIVER g_drvHandle = NULL;
CONDITION g_sdownCondition = NULL;
PTHREAD_INFO g_threads = NULL;;
UINT32 g_numOfThreads = 0;

static NETXMS_SUBAGENT_PARAM m_agentParams[] =
{
   {
      _T("DB2.Instance.Version(*)"), GetParameter, DCI_NAME_STRING[DCI_DBMS_VERSION],
      DCI_DT_STRING, _T("DB2/Instance: DBMS Version")
   }
};

static NETXMS_SUBAGENT_INFO m_agentInfo =
{
   NETXMS_SUBAGENT_INFO_MAGIC,
   SUBAGENT_NAME,
   NETXMS_VERSION_STRING,
   DB2Init, DB2Shutdown, DB2CommandHandler,
   (sizeof(m_agentParams) / sizeof(NETXMS_SUBAGENT_PARAM)), m_agentParams,
   0, NULL,
   0, NULL,
   0, NULL,
   0, NULL
};

static QUERY g_queries[] =
{
   { DCI_DBMS_VERSION, _T("SELECT service_level FROM TABLE (sysproc.env_get_inst_info())") },
   { DCI_NULL, _T("\0") }
};

static BOOL DB2Init(Config* config)
{
   AgentWriteDebugLog(7, _T("%s: initializing"), SUBAGENT_NAME);

#ifdef _WIN32
   g_drvHandle = DBLoadDriver(_T("db2.ddr"), NULL, TRUE, NULL, NULL);
#else
   g_drvHandle = DBLoadDriver(LIBDIR _T("/netxms/dbdrv/db2.ddr"), NULL, TRUE, NULL, NULL);
#endif

   if (g_drvHandle == NULL)
   {
      AgentWriteLog(EVENTLOG_ERROR_TYPE, _T("%s: failed to load the database driver"), SUBAGENT_NAME);
      return FALSE;
   }

   AgentWriteDebugLog(7, _T("%s: loaded the database driver"), SUBAGENT_NAME);

   ConfigEntry* db2IniEntry = config->getEntry(_T("/db2"));

   if (db2IniEntry == NULL)
   {
      AgentWriteLog(EVENTLOG_ERROR_TYPE, _T("%s: no entries found in the configuration file"), SUBAGENT_NAME);
      return FALSE;
   }

   PDB2_INFO* arrDb2Info;
   int numOfPossibleThreads = 0;

   ConfigEntry* xmlFile = db2IniEntry->findEntry(_T("ConfigFile"));
   if (xmlFile == NULL)
   {
      AgentWriteDebugLog(7, _T("%s: processing configuration entries in section '%s'"), SUBAGENT_NAME, db2IniEntry->getName());

      const PDB2_INFO db2Info = GetConfigs(config, db2IniEntry);
      if (db2Info == NULL)
      {
         return FALSE;
      }

      g_numOfThreads = 1;
      arrDb2Info = new PDB2_INFO[1];
      arrDb2Info[0] = db2Info;
   }
   else
   {
      const TCHAR* pathToXml = xmlFile->getValue();
      AgentWriteDebugLog(7, _T("%s: processing configuration file '%s'"), SUBAGENT_NAME, pathToXml);

      if (!config->loadXmlConfig(pathToXml))
      {
        AgentWriteLog(EVENTLOG_ERROR_TYPE, _T("%s: '%s' is not a valid configuration file"), SUBAGENT_NAME, pathToXml);
        return FALSE;
      }

      ConfigEntry* db2SubXmlEntry = config->getEntry(_T("/db2sub"));
      if (db2SubXmlEntry == NULL)
      {
         AgentWriteLog(EVENTLOG_ERROR_TYPE, _T("%s: '%s' doesn't contain the db2 configuration entry"), SUBAGENT_NAME, pathToXml);
         return FALSE;
      }

      ConfigEntryList* db2SubXmlSubEntries = db2SubXmlEntry->getSubEntries(_T("db2#*"));
      numOfPossibleThreads = db2SubXmlSubEntries->getSize();
      AgentWriteDebugLog(7, _T("%s: '%s' loaded with number of db2 entries: %d"), SUBAGENT_NAME, pathToXml, numOfPossibleThreads);
      arrDb2Info = new PDB2_INFO[numOfPossibleThreads];

      for(int i = 0; i < numOfPossibleThreads; i++)
      {
         ConfigEntry* entry = db2SubXmlSubEntries->getEntry(i);
         const PDB2_INFO db2Info = GetConfigs(config, entry);
         if (db2Info == NULL)
         {
            continue;
         }

         arrDb2Info[g_numOfThreads] = db2Info;
         g_numOfThreads++;
      }
   }

   if(g_numOfThreads > 0)
   {
      AgentWriteDebugLog(7, _T("%s: loaded %d configuration section(s)"), SUBAGENT_NAME, g_numOfThreads);
      g_threads = new THREAD_INFO[g_numOfThreads];
      g_sdownCondition = ConditionCreate(TRUE);
   }

   for(int i = 0; i < g_numOfThreads; i++)
   {
      //g_threads[i] = new THREAD_INFO;
      g_threads[i].mutex = MutexCreate();
      g_threads[i].db2Info = arrDb2Info[i];
      g_threads[i].threadHandle = ThreadCreateEx(RunMonitorThread, 0, (void*) &g_threads[i]);
   }

   delete[] arrDb2Info;

   if (g_numOfThreads > 0)
   {
      AgentWriteDebugLog(7, _T("%s: starting with %d query thread(s)"), SUBAGENT_NAME, g_numOfThreads);
      return TRUE;
   }

   return FALSE;
}

static void DB2Shutdown()
{
   AgentWriteDebugLog(9, _T("%s: terminating"), SUBAGENT_NAME);
   ConditionSet(g_sdownCondition);

   for(int i = 0; i < g_numOfThreads; i++)
   {
      ThreadJoin(g_threads[i].threadHandle);
      MutexDestroy(g_threads[i].mutex);
      delete g_threads[i].db2Info;
   }
   delete[] g_threads;

   DBUnloadDriver(g_drvHandle);
   ConditionDestroy(g_sdownCondition);

   g_drvHandle = NULL;
   g_sdownCondition = NULL;
   g_threads = NULL;

   AgentWriteDebugLog(7, _T("%s: terminated"), SUBAGENT_NAME);
}

static BOOL DB2CommandHandler(UINT32 dwCommand, CSCPMessage* pRequest, CSCPMessage* pResponse, void* session)
{
   return FALSE;
}

static THREAD_RESULT THREAD_CALL RunMonitorThread(void* info)
{
   PTHREAD_INFO threadInfo = (PTHREAD_INFO) info;
   PDB2_INFO db2Info = threadInfo->db2Info;
   DB_HANDLE dbHandle = NULL;
   TCHAR connectError[DBDRV_MAX_ERROR_TEXT];
   DWORD reconnectInterval = (DWORD) db2Info->db2ReconnectInterval;

   while(TRUE)
   {
      dbHandle = DBConnect(
         g_drvHandle, db2Info->db2DbAlias, db2Info->db2DbName, db2Info->db2UName, db2Info->db2UPass, NULL, connectError);

      if (dbHandle == NULL)
      {
         AgentWriteLog(
            EVENTLOG_ERROR_TYPE,
            _T("%s: failed to connect to the database \"%s\" (%s), reconnecting in %ds"),
            SUBAGENT_NAME, db2Info->db2DbName, connectError, reconnectInterval);

         if (ConditionWait(g_sdownCondition, (reconnectInterval * 1000)))
         {
            break;
         }
         else
         {
            continue;
         }
      }

      threadInfo->hDb = dbHandle;

      AgentWriteLog(EVENTLOG_INFORMATION_TYPE, _T("%s: connected to database '%s'"), SUBAGENT_NAME, db2Info->db2DbName);

      if (PerformQueries(threadInfo))
      {
         break;
      }
   }

   if(dbHandle != NULL)
   {
      DBDisconnect(dbHandle);
   }

   return THREAD_OK;
}

static BOOL PerformQueries(const PTHREAD_INFO threadInfo)
{
   PDB2_INFO db2Info = threadInfo->db2Info;
   DWORD queryInterval = (DWORD) db2Info->db2QueryInterval;

   while(TRUE)
   {
      MutexLock(threadInfo->mutex);

      DB_RESULT hResult;

      int i = 0;
      while(g_queries[i].dciName != DCI_NULL)
      {
         hResult = DBSelect(threadInfo->hDb, g_queries[i].query);

         if (hResult == NULL)
         {
            AgentWriteLog(EVENTLOG_ERROR_TYPE, _T("%s: query '%s' failed"), SUBAGENT_NAME, g_queries[i].query);
            continue;
         }

         TCHAR* dciName = threadInfo->db2Params[g_queries[i].dciName];

         DBGetField(hResult, 0, 0, dciName, STR_MAX);

         AgentWriteDebugLog(9, _T("%s: got '%s'"), SUBAGENT_NAME, dciName);

         DBFreeResult(hResult);

         i++;
      }

      MutexUnlock(threadInfo->mutex);

      if (ConditionWait(g_sdownCondition, queryInterval * 1000))
      {
         break;
      }
   }

   return TRUE;
}

static LONG GetParameter(const TCHAR* parameter, const TCHAR* arg, TCHAR* value)
{
   Dci dci = stringToDci(arg);

   if (dci == DCI_NULL)
   {
      return SYSINFO_RC_UNSUPPORTED;
   }

   TCHAR dbIdString[5];
   int dbId;

   AgentGetParameterArg(parameter, 1, dbIdString, STR_MAX);
   dbId = atoi((char*)dbIdString) - 1;

   if (dbId < 0 || dbId >= g_numOfThreads)
   {
      AgentWriteDebugLog(7, _T("%s: id '%s' is invalid"), SUBAGENT_NAME, dbIdString);
      return SYSINFO_RC_UNSUPPORTED;
   }

   ret_string(value, g_threads[dbId].db2Params[dci]);

   return SYSINFO_RC_SUCCESS;
}

DECLARE_SUBAGENT_ENTRY_POINT(DB2)
{
   AgentWriteDebugLog(7, _T("%s: started"), SUBAGENT_NAME);
   *ppInfo = &m_agentInfo;
   return TRUE;
}

/**
 * DLL entry point
 */

#ifdef _WIN32

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
   if (fdwReason == DLL_PROCESS_ATTACH)
   {
      DisableThreadLibraryCalls(hinstDll);
   }
   return TRUE;
}

#endif

