#include <stdio.h>
#include <winsock2.h>
#include <process.h>


#define SERVER_IP "127.0.0.1"
#define PORT 8080

void receiveMessages(SOCKET client_socket) {
    char buffer[1024];
    int bytesReceived;

    while (1) {
        bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            printf("Server: %s\n", buffer);
        } else if (bytesReceived == 0) {
            // Connection closed by the server
            printf("Server disconnected.\n");
            break;
        } else {
            // Error in receiving
            printf("Error in receiving messages.\n");
            break;
        }
    }
}

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    char client_name[100];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        WSACleanup();
        return 1;
    }

    // Prepare sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(PORT);

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Connection failed.\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Get user name
    printf("Enter your name: ");
    fgets(client_name, sizeof(client_name), stdin);
    send(client_socket, client_name, sizeof(client_name), 0);

    // Receive and print welcome message
    char welcome_msg[100];
    recv(client_socket, welcome_msg, sizeof(welcome_msg), 0);
    printf("%s\n", welcome_msg);

    // Create a thread to receive messages from the server
    _beginthread((void(*)(void*))receiveMessages, 0, (void*)client_socket);

    // Now the client can send messages to the server
    char message[1024];

    while (1) {
        printf("You: ");
        fgets(message, sizeof(message), stdin);
        send(client_socket, message, strlen(message), 0);
    }

    // Cleanup
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
