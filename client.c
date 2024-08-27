#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

void* receive_messages(void* socket_desc);

int main() {
    char *ip = "127.0.0.1";
    int port = 5566;

    int sock;
    struct sockaddr_in addr;
    pthread_t recv_thread;
    char message[BUFFER_SIZE];

    // Criação do socket do cliente
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[-] Socket Error");
        exit(1);
    }
    printf("\n[+] TCP Client Socket Created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    // Conexão com o servidor
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("[-] Connection Error");
        exit(1);
    }
    printf("\n[+] Connected to the server.\n");

    // Criação da thread para receber mensagens
    pthread_create(&recv_thread, NULL, receive_messages, (void*)&sock);

    printf("\n[+] Enter your messages below:\n");

    while (1) {
        printf("\nYou: ");
        fgets(message, BUFFER_SIZE, stdin);
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("[-] Send Error");
            exit(1);
        }
    }

    close(sock);
    return 0;
}

void* receive_messages(void* socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE];
    int n;

    while ((n = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        buffer[n] = '\0';

        // Formata a saída para ficar mais bonita
        printf("\n[Message Received]:\n%s\n", buffer);
    }

    if (n == 0) {
        printf("\n[+] Server Disconnected.\n");
    } else {
        perror("[-] Receive Error");
    }

    close(sock);
    pthread_exit(NULL);
}