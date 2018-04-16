#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

DWORD WINAPI input(void *buf) {
	char *buff = (char*)buf;
	while (TRUE) {
		gets(buf);
		if (buff[0] == 'E') break;
	}
	return 0;
}

int __cdecl main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);

	closesocket(ListenSocket);

	char number[] = "07 1231 5432 7654 2342 7623 1254";
	CreateThread(NULL, NULL, input, number, NULL, NULL);
	do {
		iResult = recv(ClientSocket, recvbuf, 1, 0);
		//printf("%s", recvbuf);
		if (iResult > 0) {
			iSendResult = send(ClientSocket, number, 33, 0); //iloœæ znaków numeru
		}
	} while (iResult > 0);

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}