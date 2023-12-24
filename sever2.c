#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>

#define PORT 5000
#define MAX_CLIENTS 10

SOCKET client_sockets[MAX_CLIENTS];
int client_count = 0;

CRITICAL_SECTION criticalSection;

unsigned __stdcall ClientHandler(void* clientSocket) {
    SOCKET client = *((SOCKET*)clientSocket);

    while (1) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // Receive message from client
        int bytesReceived = recv(client, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            printf("Client disconnected\n");
            closesocket(client);
            _endthreadex(0);
            return 0;
        }

        // Broadcast message to all clients
        EnterCriticalSection(&criticalSection);
        for (int i = 0; i < client_count; ++i) {
            if (client_sockets[i] != client) {
                send(client_sockets[i], buffer, bytesReceived, 0);
            }
        }
        LeaveCriticalSection(&criticalSection);
    }

    return 0;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }

    SOCKET server_socket;
    struct sockaddr_in server_addr;
    int addr_size = sizeof(struct sockaddr_in);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        perror("Error creating socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Error binding sock");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        perror("Error listening");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    InitializeCriticalSection(&criticalSection);

    printf("Server listening on port %d\n", PORT);

    // Accept client connections
    while (1) {
        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&server_addr, &addr_size);
        if (client_socket == INVALID_SOCKET) {
            perror("Error accepting connection");
            continue;
        }

        printf("Client connected\n");

        // Create a thread to handle the client
        _beginthreadex(NULL, 0, ClientHandler, (void*)&client_socket, 0, NULL);

        // Add client socket to the array
        EnterCriticalSection(&criticalSection);
        client_sockets[client_count++] = client_socket;
        LeaveCriticalSection(&criticalSection);
    }

    DeleteCriticalSection(&criticalSection);

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
