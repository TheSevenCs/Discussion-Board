#include "Menu.h"
#include <iostream>
#include <string>

MainMenu::MainMenu(SOCKET ClientSocket) {
    this->ClientSocket = ClientSocket;
}

void MainMenu::displayMenu() {
    int choice;
    bool isQuit = false;
    std::string author, topic, content; 
    std::vector<std::string> posts;
    std::string message;

    do {
        std::cout << "=== Main Menu ===" << std::endl;
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Create a New Account" << std::endl;
        std::cout << "3. Make a post" << std::endl;
        std::cout << "4. View posts" << std::endl;
        std::cout << "5. Send mutiple posts" << std::endl;
        std::cout << "6. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        switch (choice) {
        case 1:
            login();
            break;
        case 2:
            createAccount();
            break;
        case 3:
            std::cin.ignore();
			std::cout << "Enter the author: ";
			std::getline(std::cin, author);
			std::cout << "Enter the topic: ";
            std::getline(std::cin, topic);
			std::cout << "Enter the content: ";
            std::getline(std::cin, content);
			sendPost(author, topic, content);
			break;
        case 4:
            std::cin.ignore();
			std::cout << "Enter the author: ";
            std::getline(std::cin, author);
			std::cout << "Enter the topic: ";
            std::getline(std::cin, topic);
            // get posts
			posts = receivePosts(author, topic);
            // display posts
            for (int i = 0; i < posts.size(); i++) {
				std::cout << posts[i] << std::endl;
			}
			break;
        case 5:
       
			std::cin.ignore();
			std::cout << "How many posts would you like to send? "; 
            std::cin >> choice;
            for (int i = 0; i < choice; i++) {
                std::cin.ignore();
                std::cout << "Enter the author: ";
                std::getline(std::cin, author);
                std::cout << "Enter the topic: ";
                std::getline(std::cin, topic);
                std::cout << "Enter the content: ";
                std::getline(std::cin, content);
                message += "|POST|" + author + "|" + topic + "|" + content;
            }
            sendMessage(message);
			break;  

        case 6:
            std::cout << "Exiting the program. Goodbye!" << std::endl;
            isQuit = true;
            break;
        default:
            std::cin.ignore();
            std::cout << "Invalid choice. Exiting." << std::endl;
            isQuit = true;
            break;
        }
    } while (isQuit == false);
}

void MainMenu::sendMessage(const std::string& message) {
    send(ClientSocket, message.c_str(), message.size(), 0);
}

std::string MainMenu::receiveMessage() {
    const int bufferSize = 1024;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    int BytesReceived = recv(ClientSocket, buffer, bufferSize, 0);

    if (BytesReceived == -1)
    {
        closesocket(ClientSocket);
        WSACleanup();
        std::cout << "ERROR: failed to receive message from server" << std::endl;
        exit(-1);
    }
    return std::string(buffer);
}

void MainMenu::login() {
    std::string username, password;
    std::cout << "Enter your username: ";
    std::cin >> username;
    std::cout << "Enter your password: ";
    std::cin >> password;

    // Send login request to the server
    std::string loginRequest = "|LOGINREQ|" + username + "|" + password;
    sendMessage(loginRequest);

    // Receive and process server response
    std::string response = receiveMessage();
    if (response == "true") {
        std::cout << "Login successful!" << std::endl;
    }
    else {
        std::cout << "Login failed. Please check your credentials." << std::endl;
    }
}

void MainMenu::createAccount() {
    std::string username, password;
    std::cout << "Enter a new username: ";
    std::cin >> username;
    std::cout << "Enter a new password: ";
    std::cin >> password;

    // Send create account request to the server
    std::string createAccountRequest = "|CREATEACCOUNT|" + username + "|" + password;
    sendMessage(createAccountRequest);

    // Receive and process server response
    std::string response = receiveMessage();
    if (response == "true") {
        std::cout << "Account created successfully!" << std::endl;
    }
    else {
        std::cout << "Account creation failed. Please try a different username." << std::endl;
    }
}

void MainMenu::sendPost(std::string author, std::string topic, std::string content) {
	std::string post = "|POST|" + author + "|" + topic + "|" + content;
	sendMessage(post);
}

std::vector<std::string> MainMenu::receivePosts(std::string author, std::string topic) {
    // Send get posts request
    std::string request = "|REQFLTRD|" + author + "|" + topic;
    sendMessage(request);
    std::cout << request << std::endl;
    // Receive posts
    std::string message = receiveMessage();

    // Parse post
    std::vector<std::string> posts;
    std::string delimiter = "|POST|";
    size_t start = 0;
    size_t end = message.find(delimiter);

    while (end != std::string::npos) {
        posts.push_back(message.substr(start, end - start));
        start = end + delimiter.length();
        end = message.find(delimiter, start);
    }

    posts.push_back(message.substr(start, end));

    // output should look like this:
    // posts[0] : author1|topic1|content1
    // posts[1] : author2|topic2|content2
    // posts[2] : author3|topic3|content3
    // ...
    return posts;
}

