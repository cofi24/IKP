#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "conio.h"


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma warning(disable:4996)

#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 5059
#define BUFFER_SIZE 256



DWORD WINAPI client_read(LPVOID param) {
	SOCKET connectedSocket = (SOCKET)param;
	//check if we got data from client or EXIT signal
	//OR if we got a message from worker
	char dataBuffer[BUFFER_SIZE];
	do
	{
		int iResult = recv(connectedSocket, dataBuffer, BUFFER_SIZE, 0);
		if (iResult != SOCKET_ERROR) {
			if (iResult > 0) {
				dataBuffer[iResult] = '\0';
				// Log message text
				printf("[CLIENT]: Worker/LB sent: %s.\n", dataBuffer);
			}
			else if (iResult == 0) {
				printf("[CLIENT]: Connection closed.\n");
				break;
			}
		}
		else	// There was an error during recv
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				continue;
			}
			else {
				printf("[CLIENT]: recv failed with error: %d\n", WSAGetLastError());
				break;
			}
		}
	} while (true);
}





int main()
{

	SOCKET connectSocket = INVALID_SOCKET;

	int iResult;

	char dataBuffer[BUFFER_SIZE];

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStarup failed %d\n", WSAGetLastError());
		return 1;
	}

	connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connectSocket == INVALID_SOCKET) {
		printf("Socket failed %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;

	}


	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
	serverAddress.sin_port = htons(SERVER_PORT);



	if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	u_long non_blocking = 1;
	ioctlsocket(connectSocket, FIONBIO, &non_blocking);

	HANDLE hClientListener;
	DWORD clientID;
	hClientListener = CreateThread(NULL, 0, &client_read, (LPVOID)connectSocket, 0, &clientID);

	int msgCnt = 0;
	while (msgCnt<=50) {


		//printf("Message to send");
		//gets_s(dataBuffer, BUFFER_SIZE);
		
		Sleep(300);
		sprintf(dataBuffer, "Hello LB!!!");

		
		// Send message to server using connected socket
		iResult = send(connectSocket, dataBuffer, (int)strlen(dataBuffer), 0);
		
		if (strcmp(dataBuffer, "exit") == 0)
			break;

		msgCnt++;
		// Check result of send function
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

		printf("Message successfully sent. Total bytes: %ld\n", iResult);


	}
	Sleep(20000);
	sprintf(dataBuffer, "exit");
	// Send message to server using connected socket
	iResult = send(connectSocket, dataBuffer, (int)strlen(dataBuffer), 0);
	
	printf("\nPress any key to exit: ");
	_getch();



	// Shutdown the connection since we're done
	iResult = shutdown(connectSocket, SD_BOTH);

	// Check if connection is succesfully shut down.
	if (iResult == SOCKET_ERROR)
	{
		printf("Shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}



	closesocket(connectSocket);

	WSACleanup();

	return 0;

}

