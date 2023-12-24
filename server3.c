#include <stdio.h>
#include <winsock2.h>
#include <process.h>

#define PORT 8080
#define MAX_CLIENTS 10

// Function to handle communication with each connected client
void clientHandler(SOCKET client_socket) {
    char client_name[100];
    recv(client_socket, client_name, sizeof(client_name), 0);
    printf("%s connected.\n", client_name);

    // Send a welcome message to the client
    char welcome_msg[100];
    sprintf(welcome_msg, "Welcome, %s! You are connected.", client_name);
    send(client_socket, welcome_msg, sizeof(welcome_msg), 0);

    // Add the code for handling chat messages here
    // You may need to create a loop to continuously receive and send messages

    // Close the client socket when the communication is done
    closesocket(client_socket);
    _endthread();
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int client_count = 0;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        return 1;
    }

    // Prepare sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(server_socket);
        return 1;
    }

    // Listen
    listen(server_socket, MAX_CLIENTS);
    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connections
    while (1) {
        int addr_len = sizeof(struct sockaddr_in);
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client, &addr_len)) == INVALID_SOCKET) {
            printf("Accept failed.\n");
            continue;
        }

        // Create a thread to handle communication with the client
        _beginthread((void(*)(void*))clientHandler, 0, (void*)client_socket);

        // Increment client count
        client_count++;
    }

    // Cleanup
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
