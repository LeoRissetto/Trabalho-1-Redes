#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct
{
    int socket;
    char name[50];
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Função para lidar com a comunicação com o cliente
void *handle_client(void *client_socket);

// Função para enviar uma mensagem a todos os clientes, exceto o que a enviou
void send_message_to_all(char *message, int exclude_sock);

int main()
{
    char *ip = "127.0.0.1";
    int port = 5566;

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t tid;

    // Criação do socket do servidor
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-] Socket Error");
        exit(1);
    }
    printf("\n[+] TCP Server Socket Created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Bind do socket à porta
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("[-] Bind Error");
        exit(1);
    }
    printf("\n[+] Bind to the port number: %d.\n", port);

    // Preparação do socket para ouvir conexões
    if (listen(server_sock, 5) == 0)
    {
        printf("\n[+] Listening...\n");
    }
    else
    {
        perror("[-] Listen Error");
        exit(1);
    }

    while (1)
    {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);

        if (client_sock < 0)
        {
            perror("[-] Client Connection Failed");
            continue;
        }

        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].socket == 0)
            {
                clients[i].socket = client_sock;
                pthread_create(&tid, NULL, handle_client, (void *)&clients[i]);
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    close(server_sock);
    return 0;
}

void *handle_client(void *client_info)
{
    Client *client = (Client *)client_info;
    char buffer[BUFFER_SIZE];
    int n;

    // Solicita o nome do cliente
    send(client->socket, "Enter your name: ", 17, 0);
    n = recv(client->socket, client->name, sizeof(client->name), 0);
    client->name[n - 1] = '\0'; // Remove o newline do nome

    printf("\n[+] %s has joined the chat.\n", client->name);

    // Notifica todos os clientes sobre o novo participante
    char join_message[BUFFER_SIZE + 50];
    snprintf(join_message, sizeof(join_message), "\n[+] %s has joined the chat.\n", client->name);
    send_message_to_all(join_message, client->socket);

    while ((n = recv(client->socket, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[n] = '\0';

        char message[BUFFER_SIZE + 50];
        snprintf(message, sizeof(message), "%s: %s", client->name, buffer);

        // Exibe a mensagem no servidor com formatação
        printf("\n[Message from %s]:\n%s\n", client->name, buffer);

        pthread_mutex_lock(&mutex);
        send_message_to_all(message, client->socket);
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket == client->socket)
        {
            clients[i].socket = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(client->socket);
    printf("\n[-] %s has left the chat.\n", client->name);

    // Notifica todos os clientes que o cliente saiu
    snprintf(join_message, sizeof(join_message), "\n[-] %s has left the chat.\n", client->name);
    send_message_to_all(join_message, -1);

    pthread_exit(NULL);
}

void send_message_to_all(char *message, int exclude_sock)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket != 0 && clients[i].socket != exclude_sock)
        {
            if (send(clients[i].socket, message, strlen(message), 0) < 0)
            {
                perror("[-] Send Error");
            }
        }
    }
}