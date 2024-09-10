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
        printf("\n> ");
        fflush(stdout);
    }

    return NULL;
}

int main()
{
    int sock;
    struct sockaddr_in addr;
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

    printf("\nEnter your name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = '\0';
    send(sock, name, strlen(name), 0);

    clear_screen();

    printf("Welcome to the chat, %s!\n\n", name);
    printf("> ");
    fflush(stdout);

    pthread_create(&tid, NULL, receive_messages, &sock);

    while (1)
    {
        bzero(buffer_send, BUFFER_SIZE);
        fgets(buffer_send, BUFFER_SIZE, stdin);
        buffer_send[strcspn(buffer_send, "\n")] = '\0';
        send(sock, buffer_send, strlen(buffer_send), 0);
        clear_line();
        fflush(stdout);
    }

    close(sock);
    printf("[+]Disconnected from the Server.\n");

    return 0;
}