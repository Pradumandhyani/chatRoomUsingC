#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 5000
#define SERVER_IP "127.0.0.1"

int main() {
    // ye  Initialize krega  Windows Sockets API ko  
    // if the initialization is successful,
    //  and if not, it prints an error message and exits the program.
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
//  client_socket (socket descriptor for the client) ye ek variable h jiso humne 
// niche declare kara 
//  and server_addr store krega server ka address
    SOCKET client_socket;
    struct sockaddr_in server_addr;

    // Creating socket using socket function and agar error is there then ye error dega 
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        perror("Error creating socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    // When you set server_addr.sin_family = AF_INET;, you are indicating that the server
    //  address structure should be configured to handle IPv4 addresses.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Error connecting to server");
        closesocket(client_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    // Start chatting
    while (1) {
        char message[1024];
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        // Send message to server using send function 
        send(client_socket, message, strlen(message), 0);

        // memset is memory set and is a function in C used 
        // to fill a block of memory with a particular value. 
        memset(message, 0, sizeof(message));
        // recv is for recieve a msg 
        int bytesReceived = recv(client_socket, message, sizeof(message), 0);
        if (bytesReceived <= 0) {
            perror("Server disconnected");
            break;
        }

        printf("Server says: %s", message);
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
