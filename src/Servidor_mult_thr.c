#include "../include/comum.h"
#include "../include/Mensagens.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void usage(int argc, char **argv){
    printf("usage: %s <server IP> <server port>\n",argv[0]);
    exit(EXIT_FAILURE);
}

void logexit(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

#define BUFSZ 1024

struct client_data {
    int csock;
    struct sockaddr_storage storage;

};

void * client_thread(void *data) {
    struct client_data *cdata = (struct client_data *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&cdata->storage);
        
    char caddrstr[BUFSZ];
    addrtostr(caddr, caddrstr, BUFSZ);
    printf("\nCliente conectado.\n");   
    
    //Primeira requisição
    GameMessage request; 
    request.type = MSG_REQUEST;
    memset(&request, 0, sizeof(request));
    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    size_t count = recv(cdata->csock, &request, BUFSZ - 1, 0);

    //Não deu tempo de implementar o restante da lógica, encerrando depois do teste de conexão.
    printf("\nConexão bem-sucedida. Encerrando por aqui.\n");
    
    close(cdata->csock);
    pthread_exit(EXIT_SUCCESS);
}

// função para verificar se a entrada é válida
int mensagem_valida(int mensagem){
    if (mensagem != 0 && mensagem!=1 && mensagem != 2 && mensagem!= 3 && mensagem!=4){
        return -1;
    }else{return 1;}
}

// para determinar quem ganhou a rodada
int ganhador(int escolha_jogador){
    int escolha_servidor = rand()%5;
    if(escolha_jogador==escolha_servidor){
        return -1;
    }else if(escolha_jogador==0){
        if(escolha_servidor==2 || escolha_servidor ==3){ 
            return 1;
        }else{return 0;}
    }else if(escolha_jogador== 1){
        if(escolha_servidor==0 || escolha_servidor ==4){ 
            return 1;
        }else{return 0;}
    }else if(escolha_jogador== 2){
        if(escolha_servidor==1 || escolha_servidor ==3){ 
            return 1;
        }else{return 0;}
    }else if(escolha_jogador== 3){
        if(escolha_servidor==1 || escolha_servidor ==4){ 
            return 1;
        }else{return 0;}
    }else if(escolha_jogador== 4){
        if(escolha_servidor==2 || escolha_servidor ==0){ 
            return 1;
        }else{return 0;}
    }
}


int main(int argc, char **argv){
    if (argc < 3){
        usage(argc,argv);
    }

    struct sockaddr_storage storage;
    if(0 != server_sockaddr_init(argv[1], argv[2], &storage)){
        usage(argc, argv);
    }

    int var_socket;
    var_socket = socket(storage.ss_family, SOCK_STREAM, 0);
    if(var_socket == -1){
        logexit("socket");
    }

    int enable =1;
    if(0 != setsockopt(var_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))){
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(0 != bind(var_socket, addr, sizeof(storage))){
        logexit("bind");
    }

    if(0 != listen(var_socket, 10)){
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("Servidor iniciado em modo %.5s na porta %s. Aguardando conexão... \n", addrstr,argv[2]);

    while(1){
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr =(struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);
        int csock = accept(var_socket, caddr, &caddrlen);
        if (csock == -1){
            logexit("accept");
        }

        struct client_data *cdata = malloc(sizeof(*cdata));
        if (!cdata){
            logexit("malloc");
        }
        cdata-> csock = csock;
        memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));
        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
        
    }
    exit(EXIT_SUCCESS);
}


