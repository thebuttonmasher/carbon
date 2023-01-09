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
	/*
	* This function gets a file's content, and it's size, using winapi.
	* 
	* param sPath: char*, The path to the file to get.
	* param pSizeRead: [out] DWORD, the amount of data read from the file, no change if an error has occured.
	* 
	* return: char*, the file's content.
	*		  NULL, if an error has occured.
	*/
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
	printf("path: %s \n", sPath);
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
		printf("cant open file: INVALID HANDLE VALUE \n");
		DWORD iError = GetLastError();
		printf("getlasterror = %d", iError);

		return NULL;
	}

	nFileSize = GetFileSize(hFile, NULL); // This gives a max size of ~4.2gigs, which is more than enough.
	if (nFileSize == INVALID_FILE_SIZE)
	{
		printf("cant get file size: check permissions.");
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
			printf("can't read file");
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