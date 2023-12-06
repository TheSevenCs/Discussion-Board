/*
Ryan Tu
CSCN72020 - Networks
Fall 2023
TCP/IP Lab
Client File
*/

#include <windows.networking.sockets.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#pragma comment (lib, "Ws2_32.lib")

#define PORT 27000

int main()
{
	// create WSADATA object
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "ERROR: Failed to start WSA" << std::endl;
		return 0;
	}

	// create client socket 
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		std::cout << "ERROR: Failed to create ServerSocket" << std::endl;
		return 0;
	}

	// create connection
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(PORT);
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if ((connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
	{
		closesocket(ClientSocket);
		WSACleanup();
		std::cout << "ERROR: Connection attempted failed" << std::endl;
		return 0;
	}

	// Sending a single post to the server
	const char* post = "POST John:Dogs:I love dogs!";
	send(ClientSocket, post, strlen(post), 0);

	// Add other functionalities (sending multiple posts, requesting all posts, etc.)

	std::cout << "Messages sent" << std::endl;

	closesocket(ClientSocket);
	WSACleanup();
	return 0;

}