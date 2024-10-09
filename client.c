#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5566
#define BUFFER_SIZE 1024

char buffer_send[BUFFER_SIZE];

void clear_line()
{
    printf("\033[A");
    printf("\033[K");
}

void clear_screen()
{
    printf("\033[H\033[J");
}

// Função para gerenciar o recebimento de mensagens por parte dos clientes conectados ao server
void *receive_messages(void *sock)
{
    int server_sock = *(int *)sock;
    char buffer[BUFFER_SIZE];
    int n;

    // Recebe mensagens do servidor
    while ((n = recv(server_sock, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[n] = '\0';
        clear_line();
        printf("\n%s\n", buffer);
        printf("\n> ");
        fflush(stdout);
    }

    // Se recv() retorna 0, o servidor encerrou a conexão
    if (n == 0)
    {
        printf("\n[!] Disconnected from the server.\n");
    }
    else if (n < 0)
    {
        perror("\n[-] Error while receiving data from server");
    }

    // Finaliza o programa se a conexão for perdida
    exit(1);  // Encerra o programa após a desconexão

    return NULL;
}

int main()
{
    int sock;
    struct sockaddr_in addr;
    pthread_t tid;
    char name[50];

    // Criando um socket TCP/IP para comunicação de rede
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-] Socket Error");
        exit(1);
    }
    printf("\n[+] TCP Client Socket Created.\n");

    // Definindo a porta e os endereços de IP
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Tentando conectar o socket do cliente ao endereço do servidor
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-] Connect Error");
        exit(1);
    }
    printf("\n[+] Connected to the Server.\n");

    // Requisita o nome do cliente e envia ao servidor
    printf("\nEnter your name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = '\0';
    send(sock, name, strlen(name), 0);

    clear_screen();

    printf("Welcome to the chat, %s!\n\n", name);
    printf("> ");
    fflush(stdout);

    // Cria uma thread para lidar com o recebimento de mensagens por parte dos usuários conectados ao servidor
    pthread_create(&tid, NULL, receive_messages, &sock);
    
    // Loop para enviar as mensagens no chat
    while (1)
    {   
        bzero(buffer_send, BUFFER_SIZE);
        fgets(buffer_send, BUFFER_SIZE, stdin);
        buffer_send[strcspn(buffer_send, "\n")] = '\0';
        send(sock, buffer_send, strlen(buffer_send), 0);

        clear_line();
        fflush(stdout);
    }
    
    // Fim, fecha o socket
    close(sock);
    printf("[-] Disconnected from the Server.\n");

    return 0;
}