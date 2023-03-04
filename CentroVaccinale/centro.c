#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>


#include "list.h"


char *host_name = "127.0.0.1"; /* local host */
int port = 8000;

/*
	Nome: Francesco
	Cognome: Invitto
	Matricola: 325559
*/


int main(int argc, char *argv[]) 
{
	struct sockaddr_in serv_addr;
 	struct hostent* server;	

	itemType msg, answer;
	
	if (argc < 3){
		printf("Usage: %s nome num_pazienti\n", argv[0]);
		return -1;
	}
	
	strncpy(msg.nome, argv[1], LENMAX);
	msg.quantita = atoi(argv[2]);
	msg.min_quantita = -1;
	msg.tipo = TIPO_CENTRO;
	msg.sockfd = -1;

	if(msg.quantita <= 0){
		printf("Attenzione! Specificare un numero di pazienti > 0\n");
		return -1;
	}	

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


	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("Richiesta inviata. Rimango in attesa del nome del fornitore...\n");
	
	
	if ( recv(sockfd, &answer, sizeof(answer), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}

	printf("Nome del fornitore che soddisfa la richiesta: %s\n", answer.nome);
	
	close(sockfd);

	return 0;
}



