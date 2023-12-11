#pragma once
#include <iostream>
#include <string>
//#include <windows.networking.sockets.h>
#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <windows.h>
#include <vector>
#include <sstream>
#pragma comment (lib, "Ws2_32.lib")

#include "Client.h"

class MainMenu {
public:
    MainMenu(SOCKET);

    void displayMenu();

    void sendPost(std::string author, std::string topic, std::string content);


    std::vector<std::string> receivePosts(std::string author, std::string topic);
    

private:
    SOCKET ClientSocket;

    void sendMessage(const std::string&);

    std::string receiveMessage();

    void login();

    void createAccount();
};
