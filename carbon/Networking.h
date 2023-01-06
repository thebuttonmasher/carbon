#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <iostream>

SOCKET startup_server(PCSTR sPort);
int wait_for_command(SOCKET ListenSocket);
void cleanup_winsock(SOCKET ListenSocket);