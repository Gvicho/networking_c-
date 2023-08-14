#include <iostream>
// Dont forget to link the ws2_32.lib library to the project in which you have this code.
#include <winsock2.h>
#include <Ws2tcpip.h> // Include this header for inet_pton

int main() {
    // Step 1: Initialize WSA
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Step 2: Create a socket

    // here we specify transportation protocoles. Socket can only be one type UDP or TCP.
    // it is recommended to bind socket if you are using UDP socket and want to recive data, because In UDP ephemeral ports might be changed by OS.

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Step 3: Connect to the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345); // Port of the server
    inet_pton(AF_INET, "192.168.87.92", &(serverAddress.sin_addr));

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to the server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Step 4: Send and receive data
    char buffer[1024];
    const char* messageToSend = "Hello from client!";
    // TCP will send and then wait response from the server, while so it is on TIME_WAIT state, in which it cant establishe and accept new connections (TIME_WAIT lasts for few seconds). however programm will keep executing.
    send(clientSocket, messageToSend, strlen(messageToSend), 0);

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0'; // Null-terminate received data
        std::cout << "Received from server: " << buffer << std::endl;
    }

    // Step 5: Disconnect and close socket, cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
