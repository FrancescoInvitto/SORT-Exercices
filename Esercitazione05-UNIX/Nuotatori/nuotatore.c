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

	itemType msg;

	if (argc < 2){
		printf("Usage: %s orario\n", argv[0]);
		return -1;
	}

	if(atoi(argv[1]) < 9 || atoi(argv[1]) > 20){
		printf("Orario non compreso nella fascia 9 - 20\n");
		return -1;
	}
	
	msg.pid = getpid();
	msg.orario = atoi(argv[1]);
	msg.num_ore = 1;
	msg.tipo = TIPO_NUOTATORE;
	msg.sockfd = -1;
		
	
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


	printf("Invio al server la richiesta:\n");
	PrintItem(msg);

	/* This sends the string plus the string terminator '\0' */
	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("Richiesta inviata. In attesa di una risposta...\n");
	
	if ( recv(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}

	if(msg.pid < 0){
		printf("Nessun maestro disponibile per l'orario: %d\n", msg.orario);
	}
	else{
		printf("PID del maestro disponibile per la lezione: %d\n", msg.pid);
	}

	close(sockfd);

	return 0;
}



