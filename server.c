/*Team : Hetvi Desai, Devarshi Raval */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define SERVERPORT 8889
#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100      //Queued up connections to the server

static const char MSG_SERVER_INITIATE[] = "You can now play";
static const char MSG_SERVER_WON[] = "Game over: you won the game";
static const char MSG_SERVER_LOST[] = "Game over: you lost the game";

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;
pid_t pid;                      //Child Process PID act as Game session ID 

//child process runs the game for every two players
void servicePlayers(int client_socket_1, int client_socket_2);

// checks for socket errors
int check(int exp, const char* msg){
    if(exp == SOCKETERROR){
        perror(msg);
        exit(1);
    }
    return exp;
}

int main(int argc, char const *argv[]){
    //socket connection
    int server_socket, client_socket_1, client_socket_2, addr_size;
    SA_IN server_addr, client_addr_1, client_addr_2;
    
    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)),"Failed to create Socket");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    check(bind(server_socket, (SA *) &server_addr, sizeof(server_addr)),"Bind Failed!");
    check(listen(server_socket, SERVER_BACKLOG), "Listen Failed!");

    //waiting for players
    while (true){
        printf("Waiting for Connections...\n");
        addr_size = sizeof(SA_IN);

        check(client_socket_1 = 
                accept(server_socket, (SA*)&client_addr_1, (socklen_t*)&addr_size),
                "Accept Failed!");
        printf("Player 1 Connected\n");     //TOTO Connected
        
        check(client_socket_2 = 
                accept(server_socket, (SA*)&client_addr_2, (socklen_t*)&addr_size),
                "Accept Failed!");
        printf("Player 2 Connected\n");     //TITI Connected

        if ((pid = fork()) == 0)            //Child process handles the game
            servicePlayers(client_socket_1, client_socket_2);
    }
    return 0;
}

//child process runs the game for every two players
void servicePlayers(int client_socket_1, int client_socket_2){
    char buffer[BUFSIZE];
    size_t bytes_read;
    int dice;
    int scores[2] = {0,0};
    pid = getpid();
    printf("(%d)Game Session Initiated\n",pid);
    while (true){
        printf("(%d)Refree: TOTO Plays\n",pid);
        write(client_socket_1, MSG_SERVER_INITIATE, strlen(MSG_SERVER_INITIATE));
        bzero(buffer, BUFSIZE);
        check(read(client_socket_1,buffer,sizeof(buffer)),"RECV error");

        sscanf(buffer,"%d",&dice);
        scores[0]+=dice;
        printf("(%d)Refree: TOTO's Score is %d\n",pid,scores[0]);
        sleep(2);

        printf("(%d)Refree: TITI Plays\n",pid);
        write(client_socket_2, MSG_SERVER_INITIATE, strlen(MSG_SERVER_INITIATE));
        bzero(buffer, BUFSIZE);
        check(read(client_socket_2,buffer,sizeof(buffer)),"RECV error");
        sscanf(buffer,"%d",&dice);
        scores[1]+=dice;
        printf("(%d)Refree: TITI's Score is %d\n",pid,scores[1]);
        sleep(2);

        if(scores[0] >= 100 || scores[1] >= 100){
            if (scores[0]>scores[1]){   //TOTO Wins
                write(client_socket_1, MSG_SERVER_WON, strlen(MSG_SERVER_WON));
                write(client_socket_2, MSG_SERVER_LOST, strlen(MSG_SERVER_LOST));
                printf("(%d)Refree: TOTO Wins\n",pid);
            }
            else{                       //TITI Wins
                write(client_socket_1, MSG_SERVER_LOST, strlen(MSG_SERVER_LOST));
                write(client_socket_2, MSG_SERVER_WON, strlen(MSG_SERVER_WON));
                printf("(%d)Refree: TITI Wins\n",pid);
            }
            close(client_socket_1);     //Close TOTO's socket
            close(client_socket_2);     //Close TITI's socket
            printf("(%d)Game Session Ended\n",pid);
            return;
        }
    }
}