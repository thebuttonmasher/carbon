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

SOCKET wait_for_session(SOCKET ListenSocket) 
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
	return ClientSocket;

}

char* receive_on_socket(SOCKET ClientSocket)
{
	/*
	* This function handles receiving information from a SOCKET.
	* 
	* param ClientSocket: a SOCKET object. It should already be connected to another socket!
	* 
	* return: char*, the data recieved from the socket.
	*         NULL, if there was an error.
	*/
	char* recvbuf = (char*) malloc(BUFLEN * sizeof(char));
	int iResult;
	int recvbuflen = BUFLEN;
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0)
	{
		if (iResult < 512)
		{
			recvbuf[iResult] = '\0'; // For some reason you cant send \0 over winsock,
		}      						 // so we have to add the null byte ourself.
		std::cout << "Recieved! " << recvbuf;
		return recvbuf;
	}
	else
	{
		return NULL;
	}
}

int send_on_socket(SOCKET ClientSocket, const char* sData)
{
	/*
	* This function handles sending information to a SOCKET.
	*
	* param ClientSocket: a SOCKET object. It should already be connected to another socket!
	* param sData: const char*, the data to send through the connection.
	* 
	* return: 0 if successful, 1 if an error has occured.
	*  
	*/
	int iSendResult;
	iSendResult = send(ClientSocket, sData, (int)strlen(sData), 0);
	if (iSendResult == SOCKET_ERROR) {
		return 1;
	}
	return 0;

}

void cleanup_winsock(SOCKET ListenSocket) 
{
	/*
	* This function closes a SOCKET and calls the winsock "GC" function.
	* 
	* param ListenSocket: SOCKET, the SOCKET object you wish to close.
	*/
	closesocket(ListenSocket);
	WSACleanup();
	return;
}