#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"

#define BUF_SIZE 1000


char *host_name = "127.0.0.1"; /* local host */
int port = 8000;


int main(int argc, char *argv[]) 
{	
	struct sockaddr_in serv_addr;
 	struct hostent* server;	

	LIST titoli;
	itemType msg;

	char* endc;
	
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

	strncpy(msg.nome, "Investitore", LENMAX);
	msg.prezzo_unitario = -1;
	msg.prezzo_minimo = -1;
	msg.quantita = -1;
	msg.tipo = TIPO_INVESTITORE;
	msg.ricavo = -1;
	msg.sockfd = -1;
	msg.fine_lista = FALSE;

	printf("Presentazione alla borsa\n");

	/* This sends the string plus the string terminator '\0' */
	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("In attesa dei titoli quotati...\n");
	
	msg.fine_lista = FALSE;

	while(!msg.fine_lista){
		if ( recv(sockfd, &msg, sizeof(msg), 0) == -1 ) 
		{
			perror("Error in receiving response from server\n");
			exit(1);
		}
		PrintItem(msg);
	}

	PrintList(titoli);

	printf("\nInserire il nome del titolo desiderato: ");
	fgets(msg.nome, LENMAX, stdin);
	endc = strchr(msg.nome, '\n');
	if(endc != NULL){
		*endc = '\0';
	}
	printf("Invio della richiesta:\n");
	PrintItem(msg);

	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("\nResponse from server:\n");

	close(sockfd);

	return 0;
}



