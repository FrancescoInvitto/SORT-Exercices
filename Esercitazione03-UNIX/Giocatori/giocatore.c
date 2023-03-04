#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"

char *host_name = "127.0.0.1"; /* local host */
int port = 8000;


int main(int argc, char *argv[]) 
{
	struct sockaddr_in serv_addr;
 	struct hostent* server;	

	itemType msg, answer;
	
	if (argc < 2){ 
		printf("Usage: %s nome\n", argv[0]);
		return -1;
	}
	
	strncpy(msg.nome, argv[1], LENMAX);
	msg.num_partite = -1;
	msg.punteggio = -1;
	msg.sockfd = -1;
	msg.posizione = -1;
	msg.num_giocatore = -1;
	
	
	if ( ( server = gethostbyname(host_name) ) == 0 ) 
	{
		perror("Error resolving local host\n");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr))->s_addr;
	serv_addr.sin_port = htons(port);
	
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket\n");
		exit(1);
	}    

	if ( connect(sockfd, (void*)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error connecting to socket\n");
		exit(1);
	}


	printf("Connessione al server...\n");

	/* This sends the string plus the string terminator '\0' */
	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("Connesso. In attesa di altri giocatori...\n");
	
	
	if ( recv(sockfd, &answer, sizeof(answer), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}

	if(answer.posizione == -1){
		printf("Errore: giocatore già connesso tramite altro client\n");
	}
	else{
		printf("Partita terminata. Posizione: %d°\n", answer.posizione);
	}

	close(sockfd);

	return 0;
}



