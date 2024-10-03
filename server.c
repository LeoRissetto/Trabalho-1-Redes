#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5566        // Porta para conexão do servidor
#define MAX_CLIENTS 10   // Número máximo de clientes conectados
#define BUFFER_SIZE 1024 // Tamanho do buffer para mensagens

int client_sockets[MAX_CLIENTS];                   // Array para armazenar os sockets dos clientes
char client_names[MAX_CLIENTS][50];                // Array para armazenar os nomes dos clientes
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para sincronização de threads

// Função que lida com cada cliente individualmente
void *handle_client(void *client_socket)
{
    int sock = *(int *)client_socket; // Socket do cliente
    char buffer[BUFFER_SIZE];         // Buffer para receber mensagens
    char message[BUFFER_SIZE + 50];   // Buffer para enviar nome + mensagem
    int n;
    char name[50]; // Nome do cliente

    // Recebe o nome do cliente
    bzero(name, 50);
    recv(sock, name, 50, 0);

    // Armazena o nome do cliente no array de nomes
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] == sock)
        {
            strncpy(client_names[i], name, 50); // Copia o nome para o array
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    // Notifica todos os clientes que um novo cliente entrou no chat
    snprintf(message, sizeof(message), "[%s joined the chat]", name);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] != 0 && client_sockets[i] != sock)
        {
            send(client_sockets[i], message, strlen(message), 0); // Envia a notificação para os outros clientes
        }
    }
    pthread_mutex_unlock(&mutex);

    printf("\n[+]Client %s connected.\n", name);

    // Loop para receber e transmitir mensagens
    while ((n = recv(sock, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[n] = '\0';

        // Prepara a mensagem com o nome do cliente
        snprintf(message, sizeof(message), "%s: %s", name, buffer);

        // Envia a mensagem para todos os clientes
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            send(client_sockets[i], message, strlen(message), 0);
        }
        pthread_mutex_unlock(&mutex);

        printf("\n%s\n", message); // Exibe a mensagem no servidor
    }

    // Notifica todos os clientes que o cliente saiu do chat
    snprintf(message, sizeof(message), "[%s left the chat]", name);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] != 0 && client_sockets[i] != sock)
        {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);

    // Remove o cliente da lista de clientes
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] == sock)
        {
            client_sockets[i] = 0;      // Libera o socket
            bzero(client_names[i], 50); // Limpa o nome do cliente
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(sock); // Fecha o socket do cliente
    printf("\n[+]Client %s disconnected.\n", name);
    return NULL;
}

int main()
{
    int server_sock, client_sock;                // Sockets para o servidor e clientes
    struct sockaddr_in server_addr, client_addr; // Estrutura para os endereços do servidor e cliente
    socklen_t addr_size;
    pthread_t tid; // Thread para cada cliente

    // Inicializa o array de sockets e nomes dos clientes
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
        bzero(client_names[i], 50);
    }

    // Cria o socket do servidor
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket Error");
        exit(1);
    }
    printf("\n[+]TCP Server Socket Created.\n");

    // Define o endereço do servidor
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);                   // Converte para a ordem de bytes de rede
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Endereço IP local

    // Associa o socket a um endereço e porta
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("[-]Bind Error");
        exit(1);
    }
    printf("\n[+]Bind to the port number: %d.\n", PORT);

    // Coloca o servidor em modo de escuta
    listen(server_sock, 5);
    printf("\nListening...\n");

    // Loop principal para aceitar conexões dos clientes
    while (1)
    {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0)
        {
            perror("[-]Accept Error");
            exit(1);
        }

        // Adiciona o novo socket do cliente ao array de clientes
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client_sockets[i] == 0)
            {
                client_sockets[i] = client_sock;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        // Cria uma nova thread para lidar com o cliente
        pthread_create(&tid, NULL, handle_client, &client_sock);
    }

    close(server_sock); // Fecha o socket do servidor
    return 0;
}