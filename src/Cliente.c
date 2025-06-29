#include "../include/comum.h"
#include "../include/Mensagens.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void usage(int argc, char **argv){
    printf("usage: %s <server IP> <server port>\n",argv[0]);
    exit(EXIT_FAILURE);
}

void logexit(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

#define BUFSZ 1024

int main(int argc, char **argv){
    //Verificando erros na entrada
    if (argc != 5){
        printf("Error: Invalid number of arguments\n");
    }else if(strcmp(argv[3],"-nick")!=0){
        printf("Expected '-nick' argument\n");
    }else if(strlen(argv[4])>13){
        printf("Error: Nickname too long (max 13)\n");
    }
    
    struct sockaddr_storage storage;
    if(0 != addrparse(argv[1], argv[2], &storage)){
        usage(argc, argv);
    }
    
    
    int var_socket;
    var_socket = socket(storage.ss_family, SOCK_STREAM, 0);
    if(var_socket == -1){
        logexit("socket");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);


    if(0 != connect(var_socket, addr, sizeof(storage))){
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    
    //Mensagem inicial
    struct aviator_msg msg;
    size_t recebe_start = recv(var_socket, &msg, sizeof(msg), 0);
    printf("\nRodada aberta! Digite o valor da aposta ou digite [Q] para sair (%0.f segundos restantes): ",msg.value);

    //Envia aposta
    struct aviator_msg bet_msg;  
    memset(&bet_msg, 0, sizeof(bet_msg)); 
    strcpy(bet_msg.type,"bet");
    char buf[BUFSZ];
    fgets(buf, BUFSZ-1,stdin);    
    bet_msg.value = strtof(buf, NULL);

    bet_msg.player_id = msg.player_id;
    size_t envia_aposta = send(var_socket, &bet_msg, sizeof(bet_msg), 0);



    exit(EXIT_SUCCESS);
}