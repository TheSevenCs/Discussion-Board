#pragma once

#include <winsock2.h>
#include <iostream>
//#include <windows.networking.sockets.h>
#include <fstream>
#include <windows.h>
#pragma comment (lib, "Ws2_32.lib")

#define PORT 27000

class Client {
public:
    SOCKET createConnection(void);
};