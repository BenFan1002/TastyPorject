#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

#define PORT 8080

int main(int argc, char const *argv[]) {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024] = {0};
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello World!";

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

// Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("socket failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

// Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("setsockopt failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("bind failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        printf("listen failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    int addrlen = sizeof(address);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (int *)&addrlen)) == INVALID_SOCKET) {
        printf("accept failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
    printf("%s\n", buffer);
    send(new_socket, response, strlen(response), 0);
    printf("Hello message sent\n");

    closesocket(server_fd);
    WSACleanup();
    return 0;
}