#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5566
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];    // Array que armazena os sockets dos clientes
char client_names[MAX_CLIENTS][50]; // Array que armazena os nomes dos clientes
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Função para lidar com clientes
void *handle_client(void *client_socket)
{
    int sock = *(int *)client_socket;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + 50]; // Buffer que guarda nome +
    int n;
    char name[50];

    // Recebe o nome do cliente
    bzero(name, 50);
    
    // Loop para garantir que o nome escolhido não está em uso
    while (1) {
        recv(sock, name, 50, 0);

        // Verificação do nome, se já está em uso, informa e reinicia
        int name_exists = 0;
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] != 0 && strcmp(client_names[i], name) == 0) {
                name_exists = 1;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        if (name_exists) {
            // Se o nome já existe, pede por outro
            char name_error_message[256];
            snprintf(name_error_message, sizeof(name_error_message), "Unfortunately, the username \"%s\" is already in use. Please, choose another name: ", name);
            send(sock, name_error_message, strlen(name_error_message), 0);
            bzero(name, 50);
        } else {
            // Nome único, armazenar e sair do loop
            break;
        }
    }

    // Guarda o nome do cliente no array
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] == sock)
        {
            strncpy(client_names[i], name, 50);
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    // Notifica todos os clientes sobre uma nova conexão
    snprintf(message, sizeof(message), "[%s joined the chat]", name);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] != 0 && client_sockets[i] != sock)
        {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);

    printf("\n[+] Client %s connected.\n", name);

    // Loop que recebe todas as mensagens dos clientes
    while ((n = recv(sock, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[n] = '\0';

        // Prepara a mensagem com o nome do cliente
        snprintf(message, sizeof(message), "%s: %s", name, buffer);

        // Broadcast a mensagem para todos os clients
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            send(client_sockets[i], message, strlen(message), 0);
        }
        pthread_mutex_unlock(&mutex);

        printf("\n%s\n", message);
    }

    // Notifica os clientes sobre desconexões
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

    // Remove o cliente da lista e fecha seu socket
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] == sock)
        {
            client_sockets[i] = 0;
            bzero(client_names[i], 50); // Limpa o nome do cliente
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(sock);
    printf("\n[+] Client %s disconnected.\n", name);
    return NULL;
}

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t tid;

    // Inicializa o array dos sockets dos clientes
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
        bzero(client_names[i], 50);
    }

    // Cria um socket TCP/IP para comunicação de rede
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-] Socket Error");
        exit(1);
    }
    printf("\n[+] TCP Server Socket Created.\n");

    // Definindo a porta e os endereços de IP
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Vincula o socket do servidor ao endereço IP e à porta especificados
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("[-] Bind Error");
        exit(1);
    }
    printf("\n[+] Bind to the port number: %d.\n", PORT);

    // Escuta as conexões
    listen(server_sock, 5);
    printf("\nListening...\n");

    // Loop para gerenciar as conexões dos clientes com o servidor
    while (1)
    {
        // Aceitando a conexão
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0)
        {
            perror("[-] Accept Error");
            exit(1);
        }

        // Adiciona o novo socket do cliente ao array
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

        // Cria uma thread para cada novo cliente
        pthread_create(&tid, NULL, handle_client, &client_sock);
    }

    // Fim, fecha o socket do servidor
    close(server_sock);
    return 0;
}