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
#pragma comment(lib, "Ws2_32.lib") // Required library for winsock


int parse_command(char* sRawCommand) 
{
	int iCommand;
	iCommand = (int)sRawCommand[0];
	return iCommand - 48; 
}


int do_command(int iCommand, char* args) 
{
	printf("command is %d \n", iCommand);
	return 0;

}


int command_loop()
{
	int iCommand = 0;
	char* recvbuf;
	int iCommandResult;
	int iSendResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET SessionSocket;
	ListenSocket = startup_server("27017");

	do{
		SessionSocket = wait_for_session(ListenSocket);
		recvbuf = receive_on_socket(SessionSocket);
		iCommand = parse_command(recvbuf);
		free(recvbuf); // After we parsed the command we don't need it
		iCommandResult = do_command(iCommand, NULL);
		iSendResult = send_on_socket(SessionSocket, "SUCCESS!");

	} while (iCommand > 0);
	std::cout << "done!";
	return 0;
}

int main()
{
	return command_loop();
}