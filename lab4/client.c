#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1" // IP adresa servera (localhost)
#define PORT 12345            // Port servera

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];
    char name[100];
    int recv_size;

    // Inicijalizacija WinSock-a
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Kreiranje socket-a
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // Postavljanje adrese servera
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Povezivanje na server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection to server failed\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server\n");

    // Unos imena i slanje serveru
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0'; // Uklanjanje novog reda

    char message[128];
    sprintf(message, "N%s", name); // Formatiranje poruke „Nime“
    send(client_socket, message, strlen(message), 0);

    // Primanje odgovora od servera
    recv_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (recv_size > 0) {
        buffer[recv_size] = '\0'; // Dodavanje null terminatora
        printf("Server response: %s\n", buffer);
    }

    // Čekanje na poruku „P“ od servera
    recv_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (recv_size > 0) {
        buffer[recv_size] = '\0';
        if (strcmp(buffer, "P") == 0) {
            printf("Received broadcast message 'P'\n");
            send(client_socket, "OK", 2, 0);
            printf("Sent 'OK' to server\n");
        }
    }

// Zatvaranje konekcije
    closesocket(client_socket);
    WSACleanup();
    printf("Connection closed. Press Enter to exit...\n");
    getchar(); // Čeka pritisak tipke Enter
    return 0;

    return 0;
}
