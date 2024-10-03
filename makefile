# Definição do compilador
CC = gcc

# Flags de compilação
CFLAGS = -pthread

# Nomes dos binários
SERVER_BIN = server
CLIENT_BIN = client

# Alvo padrão (compila ambos os binários)
all: server client

# Compilação do servidor
server: server.c
	$(CC) $(CFLAGS) server.c -o $(SERVER_BIN)

# Compilação do cliente
client: client.c
	$(CC) $(CFLAGS) client.c -o $(CLIENT_BIN)

# Limpeza dos binários
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)