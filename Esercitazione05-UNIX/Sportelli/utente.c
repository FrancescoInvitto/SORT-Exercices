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
	
	itemType msg;
	int i;
	
	if (argc < 3){ 
		printf("Usage: %s nome servizio\n", argv[0]);
		exit(1);
	}

	strncpy(msg.nome, argv[1], LENMAX);
	msg.servizio = atoi(argv[2]);
	msg.tipo = TIPO_UTENTE;
	msg.sockfd = -1;
	
		
	for(i = 0; i < K; ++i){
		msg.servizi[i] = 0;
	}	

	struct sockaddr_in serv_addr;
 	struct hostent* server;	
	
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


	printf("Invio del messaggio:\n");
	PrintItem(msg);

	/* This sends the string plus the string terminator '\0' */
	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("In attesa di risposta...\n");
	
	
	if ( recv(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}

	printf("Sono servito da:\n");
	PrintItem(msg);

	close(sockfd);

	return 0;
}



