# Nome do executável
SERVER_EXEC = server
CLIENT_EXEC = client

# Compilador e flags
CC = gcc
CFLAGS = -Wall -pthread

# Regras de construção
all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): server.c
	$(CC) $(CFLAGS) server.c -o $(SERVER_EXEC)

$(CLIENT_EXEC): client.c
	$(CC) $(CFLAGS) client.c -o $(CLIENT_EXEC)

# Limpa os arquivos compilados
clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC)

# Regra para recompilar tudo
rebuild: clean all

# Regra para rodar o servidor
run_server: $(SERVER_EXEC)
	./$(SERVER_EXEC)

# Regra para rodar o cliente
run_client: $(CLIENT_EXEC)
	./$(CLIENT_EXEC)