// server code 

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

bool authenticateUser(const std::string& username, const std::string& password) {
    // Simulated user authentication (replace with your authentication logic)
    // Compare received username and password with stored credentials
    return (username == "user" && password == "password");
}

void handleLoginRequest(SOCKET clientSocket, const std::string& request) {
    // Extracting username and password from the received request
    std::istringstream iss(request);
    std::string command, username, password, remove_char;
    std::getline(iss, remove_char, '|'); // Skip the first | char
    std::getline(iss, command, '|'); // Skip the command 
    std::getline(iss, username, '|');
    std::getline(iss, password, '|');

    std::cout << username << std::endl;
    std::cout << password << std::endl;


    std::string response;
    if (authenticateUser(username, password)) {
        response = "true"; // Authentication successful
    }
    else {
        response = "false"; // Authentication failed
    }

    // Send the response back to the client
    send(clientSocket, response.c_str(), response.size(), 0);
}

void handleRequestFilter(SOCKET clientSocket, const std::string& request)
{
    std::istringstream iss(request);
    std::string command, filterBy, filterValue;

    std::getline(iss, command, '|'); // Extract the command part
    std::getline(iss, command, '|');
    std::getline(iss, filterBy, '|');    // Extract the filter type (Author/Topic)
    std::getline(iss, filterValue, '|'); // Extract the filter value

    //command = command.substr(1, command.size() - 1); // Remove the leading '|'
    std::cout << command << std::endl;
        std::vector<std::string> filteredPosts;
    if (command == "REQFLTRD"){
        for (const auto& post : posts)
        {
            if (post.author == filterValue)
            {
                std::string postStr = post.author + "|" + post.topic + "|" + post.content;
                filteredPosts.push_back(postStr);
            }
        }

        for (const auto& post : posts)
        {
            if (post.topic == filterValue)
            {
                std::string postStr = post.author + "|" + post.topic + "|" + post.content;
                filteredPosts.push_back(postStr);
            }
        }
        
        // Combine all filtered posts into a single transmission
        std::string response;

        for (const auto& post : filteredPosts) {
            response += "|POST|" + post;
        }

        // Send the response back to the client
        send(clientSocket, response.c_str(), response.size(), 0);
    }
    else
    {
        // If the received command is not REQFLTRD, handle the error or invalid command here
        std::cerr << "Invalid command received" << std::endl;
        // Send an appropriate response indicating an invalid command, if needed
    }
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

        if (strncmp(buffer, "|POST|", 6) == 0) {
            std::cout << "Post detected" << std::endl;
            // Parse the received post and add it to the posts vector
            // Format: POST <Author>:<Topic>:<Content>

            std::string postStr = buffer + 6; // Skipping "|POST|"

            std::istringstream iss(postStr);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(iss, token, '|'))
            {
                tokens.push_back(token);
            }

            if (tokens.size() >= 3)
            {
                std::string author = tokens[0];
                std::string topic = tokens[1];
                std::string content = tokens[2];

                posts.push_back({ author, topic, content });
                savePostsToFile(); // Save the updated posts to file
                std::cout << "Received post: " << buffer << std::endl;
            }
            else {
                std::cout << "Invalid post format received: " << buffer << std::endl;
            }
        }


        else if (strncmp(buffer, "|LOGINREQ|", 10) == 0)
        {
            std::cout << "Login detected" << std::endl;
            handleLoginRequest(new_socket, buffer);
        }

        else if (strncmp(buffer, "|REQFLTRD|", 10) == 0)
        {
            std::cout << "Request Filtered Detected" << std::endl;
            handleRequestFilter(new_socket, buffer);
        }

        closesocket(new_socket);
    }

    WSACleanup();
    return 0;
}
