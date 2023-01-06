#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>

SOCKET startup_server(PCSTR sPort);
SOCKET wait_for_session(SOCKET ListenSocket);
void cleanup_winsock(SOCKET ListenSocket);
char* receive_on_socket(SOCKET ClientSocket);
int send_on_socket(SOCKET ClientSocket, const char* sData);