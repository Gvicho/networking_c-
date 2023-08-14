#include <iostream>
#include <winsock2.h> // don't forget to link the ws2_32.lib library to the project

using namespace std;

int main() {

    // Step 1: Initialize Winsock
    /*
    * The Windows Sockets API (WSA) allows applications to create and manage network connections and perform 
    * network communication. It's similar in functionality to the Berkeley Sockets API used on Unix-like systems.
    * Initialization of WSA (Winsock):
    * Before you start using the Winsock API in your C++ program, you need to initialize it. 
    * This initialization is done using the WSAStartup function, 
    * and it's important to call WSACleanup when you're done using the Winsock API to release resources.
    */

    cout << "Initializing Winsock\n";

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }


    // Step 2: Create socket
    
    cout << "Creating a socket!\n";

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );// here we need to specify the transportation protocole TCP or UDP
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Step 3: Bind socket
    // here we specify IP address and the port

    cout << "Binding a socket!\n";

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);  // Port number
    serverAddress.sin_addr.s_addr = INADDR_ANY; // might be : inet_addr("192.168.0.3") instead of INADDR_ANY
    /*
    * INADDR_ANY will automaticly bind socket to the any IP that is assigned by the server.
    */

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Error binding socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Step 4: Listen on the socket

    cout << "Listening on the socket!\n";

    if (listen(serverSocket, 5) == SOCKET_ERROR) { 
        // in Listen() 5 is the maximum  number of connections that can be waiting in the queue while the server is handling other connections
        // 5 would be a number of clients we answear at the time. if 6th client trys to connect it will delay or reject, until one of 5 client is released.
        // if it is too high latency is high too and server is overwhelmed
        std::cerr << "Error listening." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening for connections..." << std::endl;

    // Step 5: Accept a connection

    cout << "Accepting a connection!\n";

    SOCKET clientSocket;
    sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize); // this will wait for an incoming connection. If there are no incoming connections, the program will be in a blocked or waiting state until a connection arrives
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error accepting connection." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connection established.\n";

    // Step 6: Send and receive data (example)

    // here as you can see we are sending arrays of char. mostly the siaze of the array that we send must be less than a networks MTU
    // maximum send/recive packet size might also be dependend on the OS...
    // but if our messege is biggar than MTU, that is nomore problem because most of the OS has  Path MTU Discovery (PMTUD) to automatically handle the situation where the size of your data exceeds the Maximum Transmission Unit (MTU) 

    const char* messageToSend = "Hello from server!";
    char buffer[1024];
    int bytesReceived;

    // Send data
    send(clientSocket, messageToSend, strlen(messageToSend), 0); // here 0 in fourth argument is a flag, 0 -  No special flags, regular blocking send operation
    cout << "Sending a test message!\n";

    // Receive data
    bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0); // If there's no data available, the call will put the process in a waiting state until data arrives
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0'; // Null-terminate the received data
        std::cout << "Received: " << buffer << std::endl;
    }

    // Step 7: Disconnect

    cout << "Disconnecting socket\n";

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
