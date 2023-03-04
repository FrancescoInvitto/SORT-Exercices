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
	
	LIST utenti[K];
	int i;
	itemType item, utente, risposta;
	itemType* servito;
	int num_utenti[K];
	LIST tmp;
	
	for(i = 0; i < K; ++i){
		utenti[i] = NewList();	
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
		printf("\nWaiting for a new connection...\n");
		
		// New connection acceptance		
		int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) 
		{
			perror("Error on accept");
			exit(1);
		}
		
		// Message reception
		if ( recv( newsockfd, &item, sizeof(item), 0 ) == -1) 
		{
			perror("Error on receive");
			exit(1);
		}

		printf("Ricevuta la richiesta:\n");
		PrintItem(item);

		if(item.tipo == TIPO_UTENTE){
			printf("----- Gestione dell'utente -----\n");

			if(0 <=item.servizio && item.servizio < K){
				item.sockfd = newsockfd;
				utenti[item.servizio] = EnqueueLast(utenti[item.servizio], item);
			}
			else{
				printf("Servizio non valido %d\n", item.servizio);
				close(newsockfd);
			}
		}
		else if(item.tipo == TIPO_IMPIEGATO){
			printf("----- Gestione dell'impiegato -----\n");
			for(i = 0; i < K; ++i){
				if(item.servizi[i] != 0){
					num_utenti[i] = getLength(utenti[i]);
				}
				else{
					num_utenti[i] = 0;
				}
				
				printf("Servizio %d: ci sono %d utenti che l'impiegato potrebbe servire\n", i, num_utenti[i]);

				
				if ( send( newsockfd, num_utenti, K * sizeof(int), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
			
				for(i = 0; i < K; ++i){
					tmp = utenti[i];
					printf("Invio della lista %d\n", i);
					while(!isEmpty(tmp)){
						printf("Invio dell'item:\n");
						PrintItem(tmp->item);
	
						if ( send( newsockfd, num_utenti, K * sizeof(int), 0 ) == -1) 
						{
							perror("Error on send");
							exit(1);
						}

						tmp = tmp->next;
					}	
			}
			printf("In attesa di ricevere il cliente servito\n");
			if ( recv( newsockfd, &utente, sizeof(utente), 0 ) == -1) 
			{
				perror("Error on receive");
				exit(1);
			}

			printf("L'impiegato serve l'utente:\n");
			PrintItem(item);
			
			close(newsockfd);	
			
			//verificare che item.servizio sia compreso tra 0 e K-1

			servito = Find(utenti[utente.servizio], utente);
			strncpy(risposta.nome, item.nome, LENMAX);
			risposta.servizio = utente.servizio;
			printf("Messaggio di risposta:\n");
			PrintItem(risposta);

			if(servito != NULL){
				printf("Servito l'utente:\n");
				PrintItem(*servito);
				if ( send( servito->sockfd, &risposta, K * sizeof(risposta), 0 ) == -1) 
						{
							perror("Error on send");
							exit(1);
						}
				close(servito->sockfd);
				printf("Eliminazione dell'utente servito dalla lista\n");
				utenti[utente.servizio] = Dequeue(utenti[utente.servizio], utente);
				
			}

			else{
				printf("Non c'e' nessun utente con quel nome!\n");
			}

			for(i = 0; i < K; ++i){
				printf("Eliminazione ultimi elementi della lista %d\n", i);
				while(getLength(utenti[i]) > L){
					utenti[i] = DequeueLast(utenti[i]);
				}
			}
			
		}
		}
		else{
			printf("Tipo non valido: %d\n", item.tipo);
			close(newsockfd);
		}

		for(i = 0; i < K; ++i){
			printf("Utenti in attesa del servizio %d:\n", i);
			PrintList(utenti[i]);
		}

		/* This sends the string plus the string terminator '\0' */
		/*
		if ( send( newsockfd, buf, len + 1, 0 ) == -1) 
		{
			perror("Error on send");
			exit(1);
		}
		*/

		close(newsockfd);
	}

	for(i = 0; i < K; ++i){
		utenti[i] = DeleteList(utenti[i]);	
	}

	close(sockfd);
	return 0;
}



