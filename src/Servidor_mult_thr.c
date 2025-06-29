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
#include <math.h>



//Váriavel gloal para armazenar o númeor de jogadores
int N=0;
//Váriavel global para armazenar o valor que está sendo apostado
float V=0;


//Criando funções para  os logs:
void log_start(int num){
    printf("\nevent=start | id=* | N=%d \n",num);
}

//Calcula multiplicador máximo da rodada
float m_e(int N,float V){
    float m,gama;
    gama=0.5;
    m = pow((1 + N + (0.01*V)),gama);
    return m;
}

//Calcula o cash_out


//Para controlar o tempo:
int tempo_atual = 10 ;
pthread_mutex_t cronometro = PTHREAD_MUTEX_INITIALIZER;

void* cronometro_ativo(void* arg){
    while(1){
        sleep(1);
        pthread_mutex_lock(&cronometro);
        if (tempo_atual>0){
            tempo_atual=tempo_atual-1;
        }
        pthread_mutex_unlock(&cronometro);
        if (tempo_atual==0){
            printf("Tempo acabou\n");
            break;
        }
    }
    return NULL;
}



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
    if (N==1){
        log_start(N);
    }
    
    //Envia mensegem inicial
    struct aviator_msg start_msg;
    memset(&start_msg, 0, sizeof(start_msg));
    pthread_mutex_lock(&cronometro);
    start_msg.value = tempo_atual;
    pthread_mutex_unlock(&cronometro);
    strcpy(start_msg.type,"start");
    start_msg.player_id = N;
    size_t envia_start = send(cdata->csock, &start_msg, sizeof(start_msg), 0);
    
    //Recebe a aposta
    struct aviator_msg msg;
    size_t recebe_aposta = recv(cdata->csock, &msg, sizeof(msg), 0);
    if (msg.player_id==-1) {
        N=N-2;
    }
    V = msg.value+V;
    printf("\nevent=%s | id=%d | N=%d | V=%.00f  \n",msg.type,msg.player_id,N,V);

    pthread_mutex_lock(&cronometro);
    if (tempo_atual==0){
    //Avisa que o tempo acabou
    struct aviator_msg closed_msg;
    memset(&closed_msg, 0, sizeof(closed_msg));
    strcpy(closed_msg.type,"closed");
    size_t envia_closed = send(cdata->csock, &closed_msg, sizeof(closed_msg), 0);   
    }
    pthread_mutex_unlock(&cronometro);

    close(cdata->csock);
    pthread_exit(EXIT_SUCCESS);
}

// função para verificar se a entrada é válida
int mensagem_valida(int mensagem){
    if (mensagem != 0 && mensagem!=1 && mensagem != 2 && mensagem!= 3 && mensagem!=4){
        return -1;
    }else{return 1;}
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

    while(1){
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr =(struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);
        int csock = accept(var_socket, caddr, &caddrlen);

        N=N+1;


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
        
        pthread_create(&tid, NULL, cronometro_ativo, NULL);

        pthread_create(&tid, NULL, client_thread, cdata);
 
    }

}