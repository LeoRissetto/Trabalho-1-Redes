# Projeto de Chat Cliente-Servidor em C

Projeto 1 da disicplina de Redes de Computadores, ministrada pelo Prof. Dr. Rodolfo Ipolito Meneguette.

### Membros do Grupo

- Leonardo Gueno Rissetto (13676482)
- Lucas Lima Romero (13676325)
- Luciano Gonçalves Lopes Filho (13676520)
- Marco Antonio Gaspar Garcia (11833581)
- Thiago Kashivagui Gonçalves (13676579)

## 1. Resumo
Este projeto implementa um sistema de chat em tempo real utilizando a arquitetura cliente-servidor em C. O servidor é capaz de gerenciar múltiplos clientes simultaneamente, permitindo que enviem e recebam mensagens entre si. O projeto explora o uso de sockets e threads para comunicação eficiente e sincronização entre os clientes.

## 2. Objetivos
O objetivo deste projeto é desenvolver um sistema de chat que permita a comunicação em tempo real entre múltiplos usuários. O sistema foi desenvolvido para aprimorar o entendimento sobre a implementação de protocolos de comunicação e o uso de threads em C.

Mais Especificamente, os objetivos deste projeto são:
- Implementar um servidor que aceita conexões de múltiplos clientes;
- Permitir que clientes se comuniquem entre si enviando mensagens;
- Gerenciar a sincronização de mensagens entre diferentes clientes utilizando mutexes.

## 3. Requisitos
### Materiais utilizados:
- **Linguagem de programação**: C
- **Bibliotecas utilizadas**: `<stdio.h>`, `<string.h>`, `<stdlib.h>`, `<unistd.h>`, `<arpa/inet.h>`, `<pthread.h>`
- **Sistemas Operacionais suportados:** MacOS e Linux (Ubuntu 20.04)

## 4. Descrição
### Servidor:
- O servidor é iniciado e aguarda conexões na porta especificada.
- Cada cliente conectado é gerenciado em uma thread separada, permitindo que múltiplos clientes se comuniquem simultaneamente.
- Mensagens recebidas de um cliente são retransmitidas a todos os outros clientes conectados.

### Cliente:
- O cliente se conecta ao servidor e solicita um nome de usuário.
- Após a conexão, o cliente pode enviar mensagens que são visíveis para todos os outros clientes.
- Mensagens recebidas são exibidas em tempo real.

## 5. Instruções de Compilação
Para compilar o projeto, utilize o Makefile incluído. Execute os seguintes comandos no terminal:
```
make          # Compila ambos os binários (servidor e cliente)
```
```
make clean    # Remove os binários compilados
```

## 6. Execução

Para executar o servidor e o cliente, siga os passos abaixo:

1. No terminal, inicie o servidor:
```
./server
```

2. Em outros terminais, inicie o cliente:
```
./client
```

3. Digite um nome de usuário e comece a enviar suas mensagens! O nome escolhido não pode ser o mesmo de alguém que já esteja na sala.

