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
    if (argc < 3){
        usage(argc,argv);
    }
    
    struct sockaddr_storage storage;
    if(0 != addrparse(argv[1], argv[2], &storage)){
        usage(argc, argv);
    }
    
    
    int var_socket;
    var_socket = socket(storage.ss_family, SOCK_STREAM, 0);
    if(socket == -1){
        logexit("socket");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);


    if(0 != connect(var_socket, addr, sizeof(storage))){
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("\nConectado ao servidor.\n");
    //Primeira resposta
    GameMessage resposta; 
    memset(&resposta, 0, sizeof(resposta));     
    resposta.type = MSG_RESPONSE;
    
    printf("\nEscolha sua jogada: \n\n0 - Nuclear Attack \n1 - Intercept Attack \n2 - Cyber Attack \n3 - Drone Strike \n4 - Bio Attack\n\n");
 
    char buf[BUFSZ];    
    fgets(buf, BUFSZ-1,stdin);   
    size_t count = send(var_socket, buf, strlen(buf)+1, 0);
    if (count != strlen(buf)+1){
        logexit("send");
    }


    memset(buf, 0, BUFSZ);
    unsigned total = 0;
    while(1){
        count = recv(var_socket, buf + total, BUFSZ - total, 0);
        if(count==0){
            break;
        }
        total += count;
    }


    puts(buf);

    exit(EXIT_SUCCESS);
}