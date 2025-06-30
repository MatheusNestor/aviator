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

    //Verificando erros na entrada

    //Mensagem inicial
    struct aviator_msg msg;
    size_t recebe_start = recv(var_socket, &msg, sizeof(msg), 0);
    printf("\nRodada aberta! Digite o valor da aposta ou digite [Q] para sair (%0.f segundos restantes): ",msg.value);

    int verifica_q =1;    
    while (verifica_q){    
        //Envia aposta
        struct aviator_msg bet_msg;  
        memset(&bet_msg, 0, sizeof(bet_msg)); 
        strcpy(bet_msg.type,"bet");
        char buf[BUFSZ];
        fgets(buf, BUFSZ-1,stdin);   
        
        
        //Confere se o player quer sair
        if(strcmp(buf,"Q\n")==0){
            verifica_q=0;
            printf("Aposte com responsabilidade. A plataforma é nova e tá com horário bugado. Volte logo, Flop.\n");
            send(var_socket, &bet_msg, sizeof(bet_msg), 0);
            close(var_socket);
            exit(EXIT_SUCCESS);
        }else if(strcmp(buf,"C\n")==0){
            struct aviator_msg cash_msg;  
            memset(&cash_msg, 0, sizeof(cash_msg)); 
            strcpy(cash_msg.type,"cashout");
            char buf[BUFSZ];
            fgets(buf, BUFSZ-1,stdin);   
        }
        //Trata valor inválidos de entrada
        else if(strtof(buf, NULL)<=0){
            printf("Error: Invalid bet value\n");
            close(var_socket);
            exit(EXIT_SUCCESS);
        }else if(strtof(buf, NULL)>0 && msg.value>0){
            bet_msg.value = strtof(buf, NULL);
            bet_msg.player_id = msg.player_id;
            size_t envia_aposta = send(var_socket, &bet_msg, sizeof(bet_msg), 0);
            printf("Aposta recebida: R$ %.00f\n",bet_msg.value);
        }else{
            printf("Error: Invalid command\n");
            close(var_socket);
            exit(EXIT_SUCCESS);    
        }

        //recebe closed inicial
        struct aviator_msg closed_msg;
        size_t recebe_closed = recv(var_socket, &closed_msg, sizeof(closed_msg), 0);
        printf("\nApostas encerradas! Não é mais possível apostar nesta rodada. Digite [C] para sacar.\n");

        //Envia pedido de cash
        struct aviator_msg cash_msg;  
        memset(&cash_msg, 0, sizeof(cash_msg)); 
        strcpy(cash_msg.type,"cashout");
        char buf2[BUFSZ];
        fgets(buf2, BUFSZ-1,stdin);   
        cash_msg.value = strtof(buf2, NULL);
        cash_msg.player_id = msg.player_id;
        size_t envia_cash = send(var_socket, &cash_msg, sizeof(cash_msg), 0);
        
        //Encerra o loop
        verifica_q =0;
    }
    close(var_socket);
    exit(EXIT_SUCCESS);   
}