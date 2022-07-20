# Internet Relay Chat

**SSC0142 - Redes de Computadores** 

- Matheus Ventura de Sousa         - 11345541
- Gustavo Henrique Brunelli        - 11801053
- Pedro Lucas de Moliner de Castro - 11795784

> Este trabalho está disponível, também, no [GitHub](https://github.com/pedrolmcastro/chat).


## Dependências

O projeto foi construído na linguagem **C++ 20**, compilado com **g++** e **Makefile** em um sistema operacional **Ubuntu 20.04** e utiliza as bibliotecas unistd.h, arpa/inet.h, sys/socket.h e netinet/in.h dependentes do sistema operacional para implementação dos sockets, além da ncurses.h para manipulação do terminal.

```
sudo apt install g++
sudo apt install make
sudo apt install libncurses-dev
```


## Compilação e Execução

Para compilar o trabalho, basta entrar no diretório **src/** e invocar a diretiva `make`, ao fim de sua execução será possível notar o aparecimento dos arquivos executáveis **client/client.out** e **server/server.out**.

```
cd src/
make
```

Para executar o Cliente, basta rodar o comando `make client` e, similarmente, para rodar um Servidor a diretiva `make server` é utilizada. Recomenda-se a utilização de multiplos terminais separados para melhor visualização da interação entre os programas.


## Utilização

Um **Servidor**, ao ser inicilizado, fica aguardando a recepção de **Clientes**, estes podem se conectar utilizando `/connect` seguido de um `/join` para adentrar um canal, a partir dai será possível enviar mensagens que serão replicadas para todos os participantes do canal, havendo a possibilidade de que a mensagem seja particionada se adequando a blocos de _4096 bytes_. Para encerrar a comunicação basta usar o comando `/quit`.

Comando           | Descrição
:---------------- | :--------------------------------------------------
/clear            | Limpa a janela de chat
/help             | Exibe a lista de comandos
/quit (Ctrl + D)  | Encerra a execução do programa
/connect IP       | Conecta-se com o servidor ouvindo em IP:6667
/nickname APELIDO | Altera seu apelido para APELIDO
/join CANAL       | Entra no canal CANAL, possivelmente o criando
/ping             | Solicita que o servidor responda "Pong"
/kick APELIDO     | Expulsa o cliente identificado por APELIDO do canal
/mute APELIDO     | Muta o cliente identificado por APELIDO no canal
/whois APELIDO    | Solicita o ip do cliente identificado por APELIDO
/unmute APELIDO   | Desmuta o cliente identificado por APELIDO no canal

> **Nota:** A impressão de mensagens muito longas em janelas de terminais pequenas pode bagunçar o funcionamento da biblioteca ncurses.h, mas basta aumentar o tamanho da janela e resetar o programa que será possível vizualizar as mensagens corretamente, inclusive sua segmentação no caso de passar do tamanho limite estabelecido.


## Implementação

Na primeira entrega, um sistema com **múltiplas threads** foi aplicado, onde, no cliente, uma thread ficava responsável por ouvir o teclado e enviar mensagens enquanto a outra recebia as mensagens do socket. Porém, para que fosse possível que qualquer uma das threads sinalizasse o fim da execução para a outra, as operações foram transformadas em **não bloqueantes**, o que efetivamente gerou uma **espera ocupada** e, portando, o alto uso da CPU.

Para corrigir o problema, na segunda e última entrega, foi utilizado o sistema de eventos em descritores de arquivos **epoll** dos sistemas Linux, abstraído na classe `Event` com o auxilio de outras estruturas orientadas a eventos, como uma fila customizada implementada na classe `Message::Queue` e métodos `send()` e `receive()` do cliente que recebem uma função lambda como parâmetro para lidar com falhas. Dessa forma é possível detectar se um socket está disponível para leitura ou escrita de dados e executar as ações apropriadas utilizando somente **uma thread**.

> **Formatação das mensagens:** Cor Apelido Mensagem \n
