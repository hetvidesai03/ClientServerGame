/*Team : Hetvi Desai, Devarshi Raval */ 
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SERVER_PORT 8889
static const char SERVER[] = "127.0.0.1";

#define MAXLINE 4096
#define SOCKETERROR (-1)
#define SA struct sockaddr

static const char MSG_SERVER_INITIATE[] = "You can now play";
static const char MSG_SERVER_WON[] = "Game over: you won the game";
static const char MSG_SERVER_LOST[] = "Game over: you lost the game";
static const char MSG_PLAYER_WON[] = "I won the game";
static const char MSG_PLAYER_LOST[] = "I lost the game";

// checks for socket errors
int check(int exp, const char* msg){
    if(exp == SOCKETERROR){
        perror(msg);
        exit(1);
    }
    return exp;
}

//Game Logic
void play_game(int sockfd){
	char buffer[MAXLINE];
	size_t bytes_read;
	long int ss = 0;
	int dice;
	while (true){
		bzero(buffer,MAXLINE);
		check(read(sockfd,buffer,sizeof(buffer)),"RECV error");
		
		if(strcmp(buffer,MSG_SERVER_INITIATE) == 0){		//"You can now Play"
			dice = (int)time(&ss) % 10 + 1; 				//Dice Logic
			printf("I got %d\n",dice);
			sprintf(buffer, "%d", dice);
			write(sockfd,buffer,sizeof(buffer));
		}
		else if (strcmp(buffer,MSG_SERVER_WON) == 0){		//"Game Over Won"
			printf(MSG_PLAYER_WON);
			exit(0);
		}
		else if (strcmp(buffer,MSG_SERVER_LOST) == 0){		//"Game Over Lost"
			printf(MSG_PLAYER_LOST);
			exit(0);
		}
	}
}

int main(int argc, char **argv){
	int sockfd;
	struct sockaddr_in servaddr;
	
	check((sockfd = socket(AF_INET, SOCK_STREAM, 0)),"Failed to create Socket!");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	
	check(inet_pton(AF_INET,SERVER, &servaddr.sin_addr),"PTON error!");
	check(connect(sockfd, (SA*) &servaddr, sizeof(servaddr)),"Connect failed!");
	
	printf("Connected to Server\n");
	
	play_game(sockfd);
	close(sockfd);

	printf("\nDisconnected from Server\n");
}