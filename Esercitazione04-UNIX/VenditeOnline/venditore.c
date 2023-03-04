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
	int accettata;
	
	if (argc < 5){ 
		printf("Usage: %s nome codice_prodotto quantità prezzo_unitario\n", argv[0]);
		return -1;
	}

	if(atoi(argv[2]) < 0 || atoi(argv[2]) >= N){
		printf("Codice prodotto non valido! Deve essere compreso tra 0 e %d\n", N-1);
		return -1;
	}

	strncpy(msg.nome, argv[1], LENMAX);
	msg.prodotto = atoi(argv[2]);
	msg.quantita = atoi(argv[3]);
	msg.prezzo_unitario = atoi(argv[4]);
	msg.tipo = TIPO_VENDITORE;
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


	printf("Invio al server l'offerta del prodotto:\n");
	PrintItem(msg);

	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	if ( recv(sockfd, &accettata, sizeof(int), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}
	
	if(accettata == 0){
		printf("Offerta rifiutata. Termino la mia esecuzione\n");
		close(sockfd);
		return 0;
	}

	printf("Offerta accettata. In attesa di vendere tutti i prodotti...\n");
	
	while(msg.quantita > 0){
		if ( recv(sockfd, &msg, sizeof(msg), 0) == -1 ) 
		{
			perror("Error in receiving response from server\n");
			exit(1);
		}
	}

	printf("Tutti i prodotti sono stati venduti. Termino la mia esecuzione\n");
	

	close(sockfd);

	return 0;
}



