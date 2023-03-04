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
	itemType msg, item, first;
	int i, j, servizio;
	int num_utenti[K];
	
	if (argc < 3) {
		printf("Usage: %s nome [codici dei servizi svolti]\n", argv[0]);
		return -1;
	}
	
	strncpy(msg.nome, argv[1], LENMAX);
	msg.servizio = -1;
	
	for(i = 0; i < argc; ++i){
		msg.servizi[i] = 0;
	}

	for(i = 0; i < argc; ++i){
		servizio = atoi(argv[i]);
		if(0 <= servizio && servizio < K){
			msg.servizi[servizio] = 1;
		}
	}
	
	msg.servizio = -1;
	msg.tipo = TIPO_IMPIEGATO;
	msg.sockfd = -1;
	
		
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


	printf("Invio della richiesta...\n");

	/* This sends the string plus the string terminator '\0' */
	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("In attesa della risposta...\n");
	
	
	if ( recv(sockfd, num_utenti, K * sizeof(int), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}
	
	first.servizio = -1;
	for(i = 0; i < K; ++i){
		printf("Servizio %d - %d utenti in attesa:\n", i, num_utenti[i]);

		for(j = 0; j < num_utenti[i]; ++j){
			if ( recv(sockfd, &item, sizeof(item), 0) == -1 ) 
			{
				perror("Error in receiving response from server\n");
				exit(1);
			}
			PrintItem(item);

			if(first.servizio == -1){	//alternativa: si selezionano i primi di ogni lista e poi si sceglie a caso tra quelli
				strncpy(first.nome, item.nome, LENMAX);
				first.servizio = item.servizio;
				//riempire gli altri campi
			}
		}
	}

	
	printf("L'utente servito e':\n");
	PrintItem(first);

	if ( send(sockfd, &first, sizeof(first), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("Inviato l'utente, termino l'esecuzione\n");

	close(sockfd);

	return 0;
}



