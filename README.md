# Projeto de Cliente-Servidor em C

## Disciplina: Redes de Computadores

## Membros do Grupo

- Leonardo Gueno Rissetto (13676482)
- Lucas Lima Romero (13676325)
- Luciano Gonçalves Lopes Filho (13676520)
- Marco Antonio Gaspar Garcia (11833581)
- Thiago Kashivagui Gonçalves (13676579)

## 1. Resumo
Este projeto implementa um sistema de chat em tempo real utilizando a arquitetura cliente-servidor em C. O servidor é capaz de gerenciar múltiplos clientes simultaneamente, permitindo que enviem e recebam mensagens entre si. O projeto explora o uso de sockets e threads para comunicação eficiente e sincronização entre os clientes.

## 2. Objetivos
O objetivo deste projeto é desenvolver um sistema de chat que permita a comunicação em tempo real entre múltiplos usuários. O sistema foi desenvolvido para aprimorar o entendimento sobre a implementação de protocolos de comunicação e o uso de threads em C.

### Específicos:
- Implementar um servidor que aceita conexões de múltiplos clientes.
- Permitir que clientes se comuniquem entre si enviando mensagens.
- Gerenciar a sincronização de mensagens entre diferentes clientes utilizando mutexes.

## 3. Materiais e Métodos
### Materiais:
- **Linguagem de programação**: C
- **Bibliotecas utilizadas**: `<stdio.h>`, `<string.h>`, `<stdlib.h>`, `<unistd.h>`, `<arpa/inet.h>`, `<pthread.h>`

### Métodos:
O projeto consiste em duas partes principais: o servidor e o cliente. 
- **Servidor**: Escuta por conexões de clientes e gerencia a comunicação entre eles.
- **Cliente**: Conecta ao servidor, envia mensagens e recebe mensagens de outros clientes.

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

make          # Compila ambos os binários (servidor e cliente)

make clean    # Remove os binários compilados

## 6. Execução

Para executar o servidor e o cliente, siga os passos abaixo:

1. No terminal, inicie o servidor:
   
./server

2. Em outro terminal, inicie o cliente:

./client

3. Siga as instruções no terminal do cliente para entrar no chat.

## 7. Conclusão

Este projeto demonstrou como implementar um sistema de chat em tempo real usando sockets e threads em C. Ele fornece uma base sólida para o entendimento de comunicação em rede e gerenciamento de múltiplas conexões.


