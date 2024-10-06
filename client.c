#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5566        // Porta do servidor
#define BUFFER_SIZE 1024 // Tamanho do buffer para mensagens

char buffer_send[BUFFER_SIZE]; // Buffer para enviar mensagens

// Função para limpar a linha anterior
void clear_line()
{
    printf("\033[A");  // Move o cursor duas linhas para cima
    printf("\033[K");   // Limpa a linha atual
}

// Função para limpar a tela
void clear_screen()
{
    printf("\033[H\033[J");
}

// Função que recebe mensagens do servidor em uma thread separada
void *receive_messages(void *sock)
{
    int server_sock = *(int *)sock;
    char buffer[BUFFER_SIZE]; // Buffer para receber mensagens
    int n;

    // Loop para receber mensagens do servidor
    while ((n = recv(server_sock, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[n] = '\0'; // Termina a string recebida
        clear_line();
        printf("%s\n", buffer); // Exibe a mensagem recebida
        printf("\n> ");
        fflush(stdout); // Limpa o buffer de saída
    }

    return NULL;
}

int main()
{
    int sock;
    struct sockaddr_in addr; // Estrutura para o endereço do servidor
    pthread_t tid;           // Thread para receber mensagens
    char name[50];           // Nome do cliente

    // Cria o socket do cliente
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-]Socket Error");
        exit(1);
    }
    printf("\n[+]TCP Client Socket Created.\n");

    // Define o endereço do servidor
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);                   // Define a porta do servidor
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Endereço IP local

    // Conecta ao servidor
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Connect Error");
        exit(1);
    }
    printf("\n[+]Connected to the Server.\n");

    // Solicita o nome do cliente
    printf("\nEnter your name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = '\0';  // Remove o caractere de nova linha
    send(sock, name, strlen(name), 0); // Envia o nome para o servidor

    clear_screen(); // Limpa a tela

    printf("Welcome to the chat, %s!\n\n", name);
    printf("> ");
    fflush(stdout);

    // Cria uma thread para receber mensagens
    pthread_create(&tid, NULL, receive_messages, &sock);

    // Loop principal para enviar mensagens
    while (fgets(buffer_send, BUFFER_SIZE, stdin) != NULL)
    {
        buffer_send[strcspn(buffer_send, "\n")] = '\0';  // Remove o caractere de nova linha
        send(sock, buffer_send, strlen(buffer_send), 0); // Envia a mensagem para o servidor
        fflush(stdout); // Limpa o buffer de saída
    }

    close(sock); // Fecha o socket do cliente
    return 0;
}