/*+===================================================================
  File:      Main.cpp

  Summary:   This is the file that contains the app's "main" method.
             Code execution will start and end here.
			 
===================================================================+*/
#ifndef WIN32_LEAN_AND_MEAN // Windows.h contains some winsock defs,
                            // so in order to avoid defining things twice we use this macro.
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>
#include "Networking.h"
#include "Commands.h"
#pragma comment(lib, "Ws2_32.lib") // Required library for winsock
#define BUFLEN 512

int parse_command(char* sRawCommand) 
{
	int iCommand;
	iCommand = (int)sRawCommand[0];
	return iCommand - 48; 
}


char* parse_args(char* sRawCommand)
{
	char* sArgs;
	sArgs = strtok(sRawCommand, " ");
	return sArgs;
}

int do_command(SOCKET SessionSocket, int iCommand, char* args) 
{
	char* pCommandResult;
	DWORD nSizeOfData;
	printf("command is %d \n", iCommand);
	switch (iCommand)
	{
	case 1:
		pCommandResult = get_file_content(args, &nSizeOfData);
		send_results_to_client(SessionSocket, pCommandResult, nSizeOfData);
		free(pCommandResult);
		return 0;
	}
	return NULL;

}

void send_results_to_client(SOCKET SessionSocket, char* pCommandResult, DWORD nSizeOfData)
{
	char pSendBuffer[BUFLEN];
	char* pTempCommandResult = pCommandResult;
	if (pCommandResult == NULL)
	{
		send_on_socket(SessionSocket, "ERROR! Consult Debug MSG on server.\0");
		return;
	}
	if (nSizeOfData <= BUFLEN)
	{
		send_on_socket(SessionSocket, pTempCommandResult);
	}
	else
	{
		do
		{
			memcpy(pSendBuffer, pTempCommandResult, BUFLEN);
			send_on_socket(SessionSocket, pSendBuffer);
			nSizeOfData -= BUFLEN;
			pTempCommandResult += BUFLEN;

		} while (nSizeOfData <= BUFLEN);

		if (nSizeOfData != 0)
		{
			memcpy(pSendBuffer, pTempCommandResult, nSizeOfData);
			send_on_socket(SessionSocket, pSendBuffer);
		}
	}
	return;
}


int command_loop()
{
	int iCommand = 0;
	char* recvbuf;
	char* pCommandResult;
	char* sArgs;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET SessionSocket;
	ListenSocket = startup_server("27017");

	do{
		SessionSocket = wait_for_session(ListenSocket);
		recvbuf = receive_on_socket(SessionSocket);
		iCommand = parse_command(recvbuf);
		sArgs = parse_args(recvbuf);
		do_command(SessionSocket ,iCommand, sArgs);


	} while (iCommand > 0);
	std::cout << "done!";
	return 0;
}

int main()
{
	return command_loop();
}