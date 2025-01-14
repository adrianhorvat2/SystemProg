#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345        // Port za komunikaciju
#define MAX_CLIENTS 4     // Maksimalni broj klijenata

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    int client_count = 0;
    int addr_len = sizeof(client_addr);
    char buffer[1024];

    // Inicijalizacija WinSock-a
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Kreiranje socket-a
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // Postavljanje adrese servera
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket-a na adresu
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Slušanje za klijentske konekcije
    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is running on port %d\n", PORT);

    while (1) {
        if (client_count < MAX_CLIENTS) {
            // Prihvaćanje novog klijenta
            SOCKET new_client = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
            if (new_client == INVALID_SOCKET) {
                printf("Accept failed\n");
                continue;
            }

            client_sockets[client_count++] = new_client;
            printf("Client connected, total clients: %d\n", client_count);

            // Primanje imena klijenta
            int recv_size = recv(new_client, buffer, sizeof(buffer) - 1, 0);
            if (recv_size > 0) {
                buffer[recv_size] = '\0';
                printf("Received from client: %s\n", buffer);

                // Slanje broja klijenata klijentu
                char response[128];
                sprintf(response, "B%d", client_count);
                send(new_client, response, strlen(response), 0);
            }

            // Ako se spojio četvrti klijent, šalje poruku "P" svima
            if (client_count == MAX_CLIENTS) {
                printf("All clients connected, sending 'P' to all\n");

                for (int i = 0; i < MAX_CLIENTS; i++) {
                    send(client_sockets[i], "P", 1, 0);
                }

                // Čekanje odgovora "OK" od svih klijenata
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    recv_size = recv(client_sockets[i], buffer, sizeof(buffer) - 1, 0);
                    if (recv_size > 0) {
                        buffer[recv_size] = '\0';
                        printf("Received from client %d: %s\n", i + 1, buffer);
                    }

                    // Zatvaranje konekcije
                    closesocket(client_sockets[i]);
                }

                printf("All clients disconnected, resetting server\n");
                client_count = 0;
            }
        }
    }

    // Zatvaranje server socket-a
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
