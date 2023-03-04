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

	LIST offerte;
	int num_offerte, i;

	itemType msg;

	offerte = NewList();
	
	if (argc < 2){ 
		printf("Usage: %s codice_prodotto\n", argv[0]);
		return -1;
	}
	
	if(atoi(argv[1]) < 0 || atoi(argv[1]) >= N){
		printf("Codice prodotto non valido! Deve essere compreso tra 0 e %d\n", N-1);
		return -1;
	}

	strcpy(msg.nome, "Acquirente");
	msg.prodotto = atoi(argv[1]);
	msg.quantita = 1;
	msg.tipo = TIPO_ACQUIRENTE;
	msg.prezzo_unitario = -1;
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


	printf("Mi connetto al server per la richiesta:\n");
	PrintItem(msg);


	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("In attesa di ricevere il numero delle offerte disponibili per il prodotto con codice: %d\n", msg.prodotto);
	
	if ( recv(sockfd, &num_offerte, sizeof(int), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}
		
	if(num_offerte == 0){
		printf("Nessuna offerta disponibile per il prodotto con codice %d. Termino la mia esecuzione\n", msg.prodotto);
		close(sockfd);
		return 0;
	}

	for(i = 0; i < num_offerte; i++){
		if ( recv(sockfd, &msg, sizeof(msg), 0) == -1 ) 
		{
			perror("Error in receiving response from server\n");
			exit(1);
		}
		offerte = EnqueueLast(offerte, msg);
	}
	
	printf("--- Offerte prodotto %d ---\n", msg.prodotto);
	PrintList(offerte);

	printf("Indicare il nome del venditore dal quale effettuare l'acquisto:\n");
	char venditore[LENMAX+1];
	gets(venditore);

	strncpy(msg.nome, venditore, LENMAX);

	printf("Comunico al server la scelta del venditore: %s e termino la mia esecuzione\n", msg.nome);

	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	close(sockfd);

	return 0;
}



