/* 
** NetXMS - Network Management System
** SNMP support library
** Copyright (C) 2003-2010 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: libnxsnmp.h
**
**/

#ifndef _libnxsnmp_h_
#define _libnxsnmp_h_

#include <nms_common.h>
#include <nms_util.h>
#include <nxsnmp.h>
#include <zlib.h>

#ifndef _WIN32
#include <netdb.h>
#endif


//
// Buffer structure for BER_DecodeContent for ASN_OBJECT_ID type
//

typedef struct
{
   DWORD dwLength;
   DWORD *pdwValue;
} SNMP_OID;


//
// Header structure for compiled MIB file
//

typedef struct
{
   char chMagic[6];
   BYTE bHeaderSize;    // Header size in bytes
   BYTE bVersion;
   WORD wFlags;
   BYTE bReserved[2];
   DWORD dwTimeStamp;   // Server's timestamp
} SNMP_MIB_HEADER;


//
// MIB file header constants
//

#define MIB_FILE_MAGIC     "NXMIB "
#define MIB_FILE_VERSION   1


//
// Tags for compiled MIB file
//

#define MIB_TAG_OBJECT        0x01
#define MIB_TAG_NAME          0x02
#define MIB_TAG_DESCRIPTION   0x03
#define MIB_TAG_ACCESS        0x04
#define MIB_TAG_STATUS        0x05
#define MIB_TAG_TYPE          0x06
#define MIB_TAG_BYTE_OID      0x07     /* Used if OID < 256 */
#define MIB_TAG_WORD_OID      0x08     /* Used if OID < 65536 */
#define MIB_TAG_DWORD_OID     0x09

#define MIB_END_OF_TAG        0x80


//
// Class for compressed/uncompressed I/O
//

class ZFile
{
private:
   BOOL m_bCompress;
   BOOL m_bWrite;
   FILE *m_pFile;
   z_stream m_stream;
   int m_nLastZLibError;
   int m_nBufferSize;
   BYTE *m_pDataBuffer;
   BYTE *m_pCompBuffer;
   BYTE *m_pBufferPos;

   int zwrite(void *pBuf, int nLen);
   int zputc(int ch);
   int zread(void *pBuf, int nLen);
   int zgetc(void);
   int zclose(void);

   BOOL FillDataBuffer(void);

public:
   ZFile(FILE *pFile, BOOL bCompress, BOOL bWrite);
   ~ZFile();

   size_t write(void *pBuf, int nLen) { return m_bCompress ? zwrite(pBuf, nLen) : fwrite(pBuf, 1, nLen, m_pFile); }
   int fputc(int ch) { return m_bCompress ? zputc(ch) : ::fputc(ch, m_pFile); }

   size_t read(void *pBuf, int nLen) { return m_bCompress ? zread(pBuf, nLen) : fread(pBuf, 1, nLen, m_pFile); }
   int fgetc(void) { return m_bCompress ? zgetc() : ::fgetc(m_pFile); }

   int close(void) { return m_bCompress ? zclose() : fclose(m_pFile); }

   int GetZLibError(void) { return m_nLastZLibError; }
};


//
// Functions
//

BOOL BER_DecodeIdentifier(BYTE *pRawData, DWORD dwRawSize, DWORD *pdwType, 
                          DWORD *pdwLength, BYTE **pData, DWORD *pdwIdLength);
BOOL BER_DecodeContent(DWORD dwType, BYTE *pData, DWORD dwLength, BYTE *pBuffer);
DWORD BER_Encode(DWORD dwType, BYTE *pData, DWORD dwDataLength, 
                 BYTE *pBuffer, DWORD dwBufferSize);

#endif   /* _libnxsnmp_h_ */
