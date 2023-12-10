#include "Client.h"
#include <ws2tcpip.h>

SOCKET Client::createConnection(void) {
	// create WSADATA object
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "ERROR: Failed to start WSA" << std::endl;
		exit(-1);
	}
	std::cout << "Winsock api initialized" << std::endl;

	// create client socket 
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		std::cout << "ERROR: Failed to create ServerSocket" << std::endl;
		exit(-1);
	}
	std::cout << "ClientSocket created" << std::endl;

	// create connection
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(PORT);
	// SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); -> Error C4996	
	inet_pton(AF_INET, "127.0.0.1", &SvrAddr.sin_addr); // TODO: change to server IP address

	if ((connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
	{
		closesocket(ClientSocket);
		WSACleanup();
		std::cout << "ERROR: Connection attempted failed" << std::endl;
		exit(-1);
	}

	std::cout << "Connected to server" << std::endl;

	return ClientSocket;
}
