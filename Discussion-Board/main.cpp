#include "Client.h"
#include "Menu.h"

int main(void) {
	Client newClient;

	SOCKET ClientSocket = newClient.createConnection();

	MainMenu newMenu(ClientSocket);
	newMenu.displayMenu();

	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}