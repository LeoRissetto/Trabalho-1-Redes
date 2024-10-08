#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5566
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS];
char client_names[MAX_CLIENTS][50]; // Array to store client names
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *client_socket)
{
    int sock = *(int *)client_socket;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + 50]; // Buffer to hold the name + message
    int n;
    char name[50];

    // Receive the name of the client
    bzero(name, 50);
    
    while (1) {
        recv(sock, name, 50, 0);

        // Verificar se o nome já está em uso
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
            // Nome já em uso, solicitar um novo nome
            char *error_message = "That name is already in use. Please choose another name: \n";
            send(sock, error_message, strlen(error_message), 0);
            bzero(name, 50);
        } else {
            // Nome único, armazenar e sair do loop
            break;
        }
    }

    // Store the client name in the array
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

    // Notify all clients about the new connection
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

    printf("\n[+]Client %s connected.\n", name);

    while ((n = recv(sock, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[n] = '\0';

        // Prepare the message with the client's name
        snprintf(message, sizeof(message), "%s: %s", name, buffer);

        // Broadcast the message to all clients
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            send(client_sockets[i], message, strlen(message), 0);
        }
        pthread_mutex_unlock(&mutex);

        printf("\n%s\n", message);
    }

    // Notify all clients about the disconnection
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

    // Remove the client from the list and close the socket
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_sockets[i] == sock)
        {
            client_sockets[i] = 0;
            bzero(client_names[i], 50); // Clear the client's name
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(sock);
    printf("\n[+]Client %s disconnected.\n", name);
    return NULL;
}

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t tid;

    // Initialize client sockets array
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = 0;
        bzero(client_names[i], 50);
    }

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket Error");
        exit(1);
    }
    printf("\n[+]TCP Server Socket Created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("[-]Bind Error");
        exit(1);
    }
    printf("\n[+]Bind to the port number: %d.\n", PORT);

    listen(server_sock, 5);
    printf("\nListening...\n");

    while (1)
    {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0)
        {
            perror("[-]Accept Error");
            exit(1);
        }

        // Add new client socket to the array
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

        // Create a new thread for each client
        pthread_create(&tid, NULL, handle_client, &client_sock);
    }

    close(server_sock);
    return 0;
}