/* 
** NetXMS - Network Management System
** Client Library
** Copyright (C) 2006 Victor Kirhenshtein
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
** File: mp.cpp
**
**/

#include "libnxcl.h"


//
// Create management pack file
//

DWORD LIBNXCL_EXPORTABLE NXCExportConfiguration(NXC_SESSION hSession, TCHAR *pszDescr,
                                                DWORD dwNumEvents, DWORD *pdwEventList,
                                                DWORD dwNumTemplates, DWORD *pdwTemplateList,
                                                DWORD dwNumTraps, DWORD *pdwTrapList,
                                                TCHAR **ppszContent)
{
   CSCPMessage msg, *pResponse;
   DWORD dwRqId, dwResult;

   *ppszContent = NULL;

   dwRqId = ((NXCL_Session *)hSession)->CreateRqId();

   msg.SetCode(CMD_EXPORT_CONFIGURATION);
   msg.SetId(dwRqId);
   msg.SetVariable(VID_DESCRIPTION, pszDescr);
   msg.SetVariable(VID_NUM_EVENTS, dwNumEvents);
   msg.SetVariableToInt32Array(VID_EVENT_LIST, dwNumEvents, pdwEventList);
   msg.SetVariable(VID_NUM_OBJECTS, dwNumTemplates);
   msg.SetVariableToInt32Array(VID_OBJECT_LIST, dwNumTemplates, pdwTemplateList);
   msg.SetVariable(VID_NUM_TRAPS, dwNumTraps);
   msg.SetVariableToInt32Array(VID_TRAP_LIST, dwNumTraps, pdwTrapList);
   ((NXCL_Session *)hSession)->SendMsg(&msg);

   pResponse = ((NXCL_Session *)hSession)->WaitForMessage(CMD_REQUEST_COMPLETED, dwRqId);
   if (pResponse != NULL)
   {
      dwResult = pResponse->GetVariableLong(VID_RCC);
      if (dwResult == RCC_SUCCESS)
      {
         *ppszContent = pResponse->GetVariableStr(VID_NXMP_CONTENT);
      }
      delete pResponse;
   }
   else
   {
      dwResult = RCC_TIMEOUT;
   }
   return dwResult;
}


//
// Install management pack
//

DWORD LIBNXCL_EXPORTABLE NXCImportConfiguration(NXC_SESSION hSession, TCHAR *pszContent,
                                                DWORD dwFlags, TCHAR *pszErrorText, int nErrorLen)
{
   CSCPMessage msg, *pResponse;
   DWORD dwRqId, dwResult;

   dwRqId = ((NXCL_Session *)hSession)->CreateRqId();

   msg.SetCode(CMD_IMPORT_CONFIGURATION);
   msg.SetId(dwRqId);
   msg.SetVariable(VID_NXMP_CONTENT, pszContent);
   msg.SetVariable(VID_FLAGS, dwFlags);
   ((NXCL_Session *)hSession)->SendMsg(&msg);

   pResponse = ((NXCL_Session *)hSession)->WaitForMessage(CMD_REQUEST_COMPLETED, dwRqId);
   if (pResponse != NULL)
   {
      dwResult = pResponse->GetVariableLong(VID_RCC);
      if (dwResult == RCC_SUCCESS)
      {
         *pszErrorText = 0;
      }
      else
      {
         pResponse->GetVariableStr(VID_ERROR_TEXT, pszErrorText, nErrorLen);
      }
      delete pResponse;
   }
   else
   {
      dwResult = RCC_TIMEOUT;
   }
   return dwResult;
}
