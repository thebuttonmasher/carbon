/*+===================================================================
  File:      Commands.cpp

  Summary:   This file contains code for the different commands the 
			 tool can accept.

  Functions: get_file_content();

===================================================================+*/
#include <windows.h>
#include <stdio.h>
#include "Commands.h"

#define BUFLEN 512

char* get_file_content(char* sPath, DWORD* pSizeRead)
{
	HANDLE hFile;
	int bResult;
	DWORD nBytesRead;
	DWORD nFileSize;
	char* pFileContent;
	char* pReadBuffer;

	if (!sPath)
	{
		return NULL;
	}
	hFile = CreateFileA(sPath, 
					    GENERIC_READ, // We only need read access
					    FILE_SHARE_READ, // We don't want to block other programs from accessing
										  // the file, since we're trying to be stealthy.
					    NULL,
					    OPEN_EXISTING,    // Only open the file if it exists, we don't want to create anything.
					    FILE_ATTRIBUTE_NORMAL, 
					    NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	nFileSize = GetFileSize(hFile, NULL); // This gives a max size of ~4.2gigs, which is more than enough.
	if (nFileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return NULL;
	}

	pFileContent = (char*)malloc(nFileSize);
	pReadBuffer = (char*)malloc(BUFLEN);
	char* tempFileContent = pFileContent;
	do 
	{
		bResult = ReadFile(hFile, pReadBuffer, BUFLEN, &nBytesRead, NULL);
		if (!bResult)
		{
			CloseHandle(hFile);
			return NULL;
		}
		memcpy(tempFileContent, pReadBuffer, nBytesRead); // At EOF this does nothing since nBytesRead = 0.
		tempFileContent += nBytesRead; 

	} while (bResult && nBytesRead != 0); // Read untill EOF
	*pSizeRead = nFileSize; 
	CloseHandle(hFile);
	free(pReadBuffer);
	return pFileContent;

}