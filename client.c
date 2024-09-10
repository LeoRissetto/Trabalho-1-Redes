#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5566
#define BUFFER_SIZE 1024

// Função para limpar a linha anterior no terminal
void clear_line()
{
    printf("\033[A"); // Move o cursor para a linha de cima
    printf("\033[K"); // Limpa a linha atual
}

void clear_screen()
{
    // Limpa a tela do terminal
    printf("\033[H\033[J");
}

void *receive_messages(void *sock)
{
    int server_sock = *(int *)sock;
    char buffer[BUFFER_SIZE];
    int n;

    while ((n = recv(server_sock, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[n] = '\0';
        clear_line();
        printf("\n%s\n", buffer);
        printf("\n> "); // Reexibe o prompt após uma mensagem ser recebida
        fflush(stdout);
    }

    return NULL;
}

int main()
{
    int sock;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];
    pthread_t tid;
    char name[50];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-]Socket Error");
        exit(1);
    }
    printf("\n[+]TCP Client Socket Created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Connect Error");
        exit(1);
    }
    printf("\n[+]Connected to the Server.\n");

    // Ask for the client's name
    printf("\nEnter your name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = '\0'; // Remove newline character
    send(sock, name, strlen(name), 0);

    clear_screen(); // Limpa a tela do terminal após a inserção do nome

    // Display a welcome message
    printf("Welcome to the chat, %s!\n\n", name);
    printf("> "); // Mostra o prompt de entrada de mensagem
    fflush(stdout);

    // Create a thread to receive messages from the server
    pthread_create(&tid, NULL, receive_messages, &sock);

    while (1)
    {
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        send(sock, buffer, strlen(buffer), 0);
        clear_line();
        printf("> "); // Reexibe o prompt após enviar uma mensagem
        fflush(stdout);
    }

    close(sock);
    printf("[+]Disconnected from the Server.\n");

    return 0;
}