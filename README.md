# Projeto Cliente-Servidor em C

Este projeto implementa um sistema cliente-servidor utilizando sockets TCP e threads em C. O servidor gerencia múltiplas conexões de clientes e permite que eles enviem e recebam mensagens em um chat simples, exibindo o nome de cada cliente junto com suas mensagens.

## Membros do Grupo

- Leonardo Gueno Rissetto (13676482)
- Lucas Lima Romero (13676325)
- Luciano Gonçalves Lopes Filho (13676520)
- Marco Antonio Gaspar Garcia (11833581)
- Thiago Kashivagui Gonçalves (13676579)

## Sistema Operacional e Compilador

- **Sistema Operacional:** Linux (Testado no Ubuntu 20.04)
- **Compilador:** GCC 9.4.0

## Instruções de Compilação

O projeto inclui um arquivo Makefile para facilitar a compilação do cliente e do servidor. Para compilar o sistema:

1. Abra o terminal no diretório do projeto.
2. Execute o comando:

Isso gerará dois binários: `server` e `client`.

## Instruções de Execução

### Servidor
1. Inicie o servidor executando o comando: ./server

### Cliente
1. Inicie o cliente executando o comando: ./client

Você pode iniciar múltiplos clientes, que irão se conectar ao servidor e participar do chat.

## Verificação de Falhas

### Conexão de Socket
- O código trata erros de criação de sockets, falhas ao conectar e aceitar conexões. Se ocorrer algum erro, uma mensagem será exibida no terminal, e o programa será encerrado.

### Transmissão de Informações
- Ao enviar ou receber mensagens, se houver falha na transmissão, o programa detecta a desconexão e remove o cliente da lista ativa.

## Arquivos Incluídos

- `server.c`: Código-fonte do servidor.
- `client.c`: Código-fonte do cliente.
- `Makefile`: Arquivo para compilar o projeto.
- `README.md`: Este arquivo com instruções e detalhes do projeto.

## Instruções Adicionais

- O servidor aceita até 10 clientes simultaneamente.
- Cada cliente deve inserir um nome antes de participar do chat.
- Ao desconectar, uma mensagem será enviada a todos os clientes informando a saída do usuário.


