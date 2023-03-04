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

	LIST laboratori[NUM];
	itemType msg, lab;
	int i, pid;
	LIST tmp;
	itemType *trovato;

	for(i = 0; i < NUM; i++){
		laboratori[i] = NewList();
	}

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
		printf("\nIn attesa di una nuova connessione...\n");
		
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

		printf("Nuova connessione:\n");
		PrintItem(msg);

		if(msg.tipo == TIPO_CLAB){
			printf("--- Gestione Clab ---\n");
			
			if(msg.lavorazione == A){
				printf("\nAggiungo il laboratorio nella lista delle lavorazioni A.\n");
				msg.sockfd = newsockfd;
				laboratori[A] = EnqueueLast(laboratori[A], msg);
			}
			else if(msg.lavorazione == B){
				printf("\nAggiungo il laboratorio nella lista delle lavorazioni B.\n");
				msg.sockfd = newsockfd;
				laboratori[B] = EnqueueLast(laboratori[B], msg);
			}
			else{
				printf("\nAggiungo il laboratorio nella lista delle lavorazioni C.\n");
				msg.sockfd = newsockfd;
				laboratori[C] = EnqueueOrdered(laboratori[C], msg);
			}

			printf("Metto in attesa il laboratorio...\n");
		}
		else if(msg.tipo == TIPO_CLIENTE){
			printf("--- Gestione cliente ---\n");

			printf("\nControllo se è presente almeno un laboratorio in grado di gestire la richiesta...\n");
			if(isEmpty(laboratori[msg.lavorazione])){
				printf("Nessun laboratorio presente. Rifiuto la richiesta del cliente.\n");
				msg.pid = -1;
				if ( send( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
			}
			else{
				printf("Laboratori in grado di gestire la richiesta:\n");
				PrintList(laboratori[msg.lavorazione]);
				
				printf("Seleziono il laboratorio che minimizza il numero di prodotti in sovrannumero...\n");
				
				trovato = Find(laboratori[msg.lavorazione], msg);

				printf("Laboratorio scelto:\n");
				PrintItem(*trovato);

				pid = msg.pid;
				msg.pid = trovato->pid;
				
				printf("Comunico al client il PID del laboratorio che gestirà la sua richiesta.\n");
				if ( send( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}

				printf("\nInterrompo la connessione con il client\n");
				close(newsockfd);

				printf("Comunico al laboratorio scelto il PID del client che ha effettuato la richiesta.\n");

				msg.pid = pid;
				if ( send( trovato->sockfd, &msg, sizeof(msg), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
				
				printf("\nTolgo dalla lista il laboratorio\n");
				laboratori[trovato->lavorazione] = Dequeue(laboratori[trovato->lavorazione], *trovato);
				close(trovato->sockfd);
			}
		}
		else{
			printf("Tipo %d non definito! Chiudo la connessione.\n", msg.tipo);
			close(newsockfd);
			exit(1);
		}

		printf("\n--- Laboratori A ---\n");
		PrintList(laboratori[A]);

		printf("\n--- Laboratori B ---\n");
		PrintList(laboratori[B]);

		printf("\n--- Laboratori C ---\n");
		PrintList(laboratori[C]);
	   
	}

	for(i = 0; i < NUM; i++){
		laboratori[i] = DeleteList(laboratori[i]);
	}

	close(sockfd);
	return 0;
}



