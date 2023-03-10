/*+===================================================================
  File:      Client.cpp

  Summary:   This file contains code for a client that sends commands
			 to the tool.

===================================================================+*/
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib") // Required library for winsock

#define BUFLEN 512




int __cdecl main(int argc, char** argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    char recvbuf[BUFLEN];
    int iResult;
    int recvbuflen = BUFLEN;
    const char* sendbuf;

    // Validate the parameters
    if (argc < 3) {
        printf("usage: %s server-name port-number command optional=command-args\n", argv[0]);
        printf("command list= \n \"1\" : get contents of file, path must be supplied as \"command-args\"! \n");
        return 1;
    }

    printf("server: %s \n port: %s \n command: %s \n", argv[1], argv[2], argv[3]);
    if (argv[4] != NULL)
    {
        char sTempSendBuf[BUFLEN];
        strcpy_s(sTempSendBuf, argv[3]);
        strcat_s(sTempSendBuf, " ");
        strcat_s(sTempSendBuf, argv[4]);
        strcat_s(sTempSendBuf, "\0");
        sendbuf = sTempSendBuf;
    }
    else
    {
        char sTempSendBuf[BUFLEN];
        strcpy_s(sTempSendBuf, argv[3]);
        strcat_s(sTempSendBuf, "\0");
        sendbuf = sTempSendBuf;
    }
    

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send our command
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    // Receive output from our command
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    printf("Command output: \n");
    while (iResult > 0)
    {
        if (iResult > 0)
        {
            if (iResult < 512)
            {
                recvbuf[iResult] = '\0'; // For some reason you cant send \0 over winsock,
            }      						 // so we have to add the null byte ourself.
            printf("%s", recvbuf);
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

        }
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());

        }
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;

}