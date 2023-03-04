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

	LIST centri;
	int num_centri;

	itemType msg;

	if (argc < 4){
		printf("Usage: %s nome quantita min_quantita\n", argv[0]);
		return -1;
	}
	
	strncpy(msg.nome, argv[1], LENMAX);
	msg.quantita = atoi(argv[2]);
	msg.min_quantita = atoi(argv[3]);
	msg.tipo = TIPO_FORNITORE;
	msg.sockfd = -1;
	
	if(msg.quantita <= 0){
		printf("Attenzione! Specificare quantità > 0 di vaccini del lotto!\n");
		return -1;
	}	

	if(msg.min_quantita < 0){
		printf("Attenzione! Specificare quantità minima > 0!\n");
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

	printf("Richiesta inviata. Rimango in attesa di ricevere il numero di centri vaccinali da rifornire...\n");
	
	
	if ( recv(sockfd, &num_centri, sizeof(int), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}

	printf("Numero di centri da rifornire: %d\n", num_centri);
	
	for(int i = 0; i < num_centri; i++){
		if ( recv(sockfd, &msg, sizeof(msg), 0) == -1 ) 
		{
			perror("Error in receiving response from server\n");
			exit(1);
		}

		centri = EnqueueLast(centri, msg);
	}

	printf("Centri vaccinali da servire:\n");
	PrintList(centri);
	
	close(sockfd);

	return 0;
}



