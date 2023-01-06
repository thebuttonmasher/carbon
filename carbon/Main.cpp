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


int command_loop()
{
	int iCommand;
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = startup_server("27017");

	do{

		iCommand = wait_for_command(ListenSocket);

	} while (iCommand > 0);
	std::cout << "done!";
	return 0;
}

int main()
{
	return command_loop();
}