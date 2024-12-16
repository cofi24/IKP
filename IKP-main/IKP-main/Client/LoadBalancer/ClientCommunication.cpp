#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define BUFFER_SIZE 256

DWORD WINAPI clientFunc(LPVOID lpParam) {

	int iResult;
	char dataBuffer[BUFFER_SIZE];

	SOCKET listenSocket = *(SOCKET*)lpParam;

	SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);

	if (acceptedSocket == INVALID_SOCKET) {
		printf("accept failed");
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	do {
		iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);

		if (iResult > 0) {
			dataBuffer[iResult] = '\0';
			printf("client sent %s \n", dataBuffer);
		}
		else if (iResult == 0) {
			printf("connection closed");
			closesocket(acceptedSocket);
		}
		else {
			printf("recv failed with error");
			closesocket(acceptedSocket);
		}
	} while (dataBuffer != "termite");


	return 0;
}