#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <winsock2.h>

#define PORT 27000
#define FILENAME "discussion_board.txt"

#pragma comment(lib, "ws2_32.lib")

struct Post {
    std::string author;
    std::string topic;
    std::string content;
};

std::vector<Post> posts;

void loadPostsFromFile() {
    std::ifstream file(FILENAME);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string author, topic, content;

        std::getline(iss, author, ':');
        std::getline(iss, topic, ':');
        std::getline(iss, content, ':');

        posts.push_back({ author, topic, content });
    }

    file.close();
}

void savePostsToFile() {
    std::ofstream file(FILENAME);

    for (const auto& post : posts) {
        file << post.author << ":" << post.topic << ":" << post.content << "\n";
    }

    file.close();
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in server, client;
    int opt = 1;
    int addrlen = sizeof(client);
    char buffer[1024] = { 0 };

    loadPostsFromFile();

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        std::cerr << "Setsockopt failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&client, &addrlen)) == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        recv(new_socket, buffer, sizeof(buffer), 0);

        if (strncmp(buffer, "POST", 4) == 0) {
            // Parse the received post and add it to the posts vector
            // Format: POST <Author>:<Topic>:<Content>
            std::string postStr = buffer + 5; // Skipping "POST "

            std::istringstream iss(postStr);
            std::string author, topic, content;

            std::getline(iss, author, ':');
            std::getline(iss, topic, ':');
            std::getline(iss, content, ':');

            posts.push_back({ author, topic, content });
            savePostsToFile(); // Save the updated posts to file
            std::cout << buffer << std::endl;
        }
        // Add handling for other commands (GET_ALL_POSTS, etc.)

        memset(buffer, 0, sizeof(buffer));
        closesocket(new_socket);
    }

    WSACleanup();
    return 0;
}
