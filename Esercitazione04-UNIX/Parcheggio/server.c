#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"

int port = 8000;


int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	LIST automobilisti;
	LIST posti;
	itemType msg;
	itemType* automobilista;
	itemType* posto;
	int i;

	automobilisti = NewList();
	posti = NewList();

	// Socket opening
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );  
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket");
		exit(1);
	}
	
	int options = 1;
	if(setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof (options)) < 0) {
		perror("Error on setsockopt");
		exit(1);
	}

	bzero( &serv_addr, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	// Address bindind to socket
	if ( bind( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error on binding");
		exit(1);
	}
	
	// Maximum number of connection kept in the socket queue
	if ( listen( sockfd, 20 ) == -1 ) 
	{
		perror("Error on listen");
		exit(1);
	}


	socklen_t address_size = sizeof( cli_addr );	
	
	while(1) 
	{
		printf("----- In attesa di una nuova connessione -----\n");
		
		// New connection acceptance		
		int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) 
		{
			perror("Error on accept");
			exit(1);
		}
				
		// Message reception
		if ( recv( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
		{
			perror("Error on receive");
			exit(1);
		}

		printf("Nuova richiesta ricevuta:\n"); 
		PrintItem(msg);

		if(msg.tipo == TIPO_PARCHEGGIO){
			printf("--- Gestione parcheggio ---\n");
			printf("Controllo se ci sono dei posti disponibili...\n");
			int num_posti = getLength(posti);

			if(num_posti < N){
				printf("E' presente un posto disponibile. Assegno a %s il posto numero %d\n", msg.nome, num_posti);
				msg.sockfd = newsockfd;
				msg.num_posto = num_posti;
				posti = EnqueueLast(posti, msg);

				if ( send( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}	
			}
			else{
				printf("Nessun posto disponibile. Tengo in attesa l'automobilista %s\n", msg.nome);
				msg.sockfd = newsockfd;
				automobilisti = EnqueueLast(automobilisti, msg);
			}

		}
		else if(msg.tipo == TIPO_RITIRO){
			printf("--- Gestione ritiro ---\n");

			automobilista = Find(posti, msg);
			int posto;

			if(automobilista != NULL){
				printf("L'automobilista %s ha terminato di fare shopping. Libero il parcheggio numero %d\n", automobilista->nome, automobilista->num_posto);
				posto = automobilista->num_posto;	//memorizzo il numero del posto che si è liberato
				posti = Dequeue(posti, *automobilista);
				
				if ( send( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
				close(automobilista->sockfd);

				printf("Controllo se ci sono automobilisti in attesa di un posto...\n");
				if(getLength(automobilisti) > 0){
					msg = getHead(automobilisti);
					msg.num_posto = posto;

					printf("L'automobilista %s era in attesa. Gli assegno il posto numero %d\n", msg.nome, msg.num_posto);
					if ( send( msg.sockfd, &msg, sizeof(msg), 0 ) == -1) 
					{
						perror("Error on send");
						exit(1);
					}
					automobilisti = Dequeue(automobilisti, msg);
					posti = EnqueueLast(posti, msg);
				}
				else{
					printf("Nessun automobilista in attesa\n");
				}
			}
		}
		else{
			printf("Tipo non valido\n");
			close(newsockfd);
		}

		printf("--- Parcheggio ---\n");
		PrintList(posti);
		printf("--- Automobilisti in attesa ---\n");
		PrintList(automobilisti);
		
		
	}

	automobilisti = DeleteList(automobilisti);
	posti = DeleteList(posti);

	close(sockfd);
	return 0;
}



