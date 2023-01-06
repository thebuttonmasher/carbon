/*+===================================================================
  File:      Networking.cpp

  Summary:   This file contains code that is related to the main app's
			 network communication.

  Functions: startup_server();

===================================================================+*/
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>
#include "Networking.h"

#pragma comment(lib, "Ws2_32.lib") // Required library for winsock
#define BUFLEN 512 // The length of the buffer that handles receiving bytes from a connection

SOCKET startup_server(PCSTR sPort) 
{
	/* This function starts up the winsock server and initializes
	*  everything it needs to start.
	* 
	*  param sPort: PCSTR (const char*), specifies the port the server should bind to.
	*  
	*  return: a socket object if start up was successful, NULL if an error has occured.
	*/
	if (sPort == NULL) 
	{
		PCSTR sDefaultPort = "27017";
		sPort = sDefaultPort;
	}
	int iResult;
	WSADATA wsaData;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return NULL;
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	// Define socket options as tcp/ip
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	// Try to resolve "localhost" and the port given from a string hostname into an object useable by winsock
	iResult = getaddrinfo(NULL, sPort, &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		return NULL;
	}
	// Create Server socket
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		return NULL;
	}
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return NULL;
	}
	return ListenSocket;
}

int wait_for_command(SOCKET ListenSocket) 
{
	/* This function starts listening for communication 
	*  on the ListenSocket Socket.
	*  And parses the messages recieved into commands,
	*  that are then returned.
	* 
	*  param ListenSocket: a SOCKET object. It should already be initialized and bound.
	* 
	*  return: A command opcode, or NULL if an error has occured.
	*/

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		return NULL;
	}
	// We need to create a temporary SOCKET object to accept a connection.
	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;
	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		return NULL;
	}

	char recvbuf[BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = BUFLEN;
	// Receive until the peer shuts down the connection
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0)
	{
		std::cout << "Recieved! " << recvbuf;
		closesocket(ClientSocket);
		return 1;
	}
	else
	{
		return NULL;
	}

}

void cleanup_winsock(SOCKET ListenSocket) 
{
	closesocket(ListenSocket);
	WSACleanup();
	return;
}