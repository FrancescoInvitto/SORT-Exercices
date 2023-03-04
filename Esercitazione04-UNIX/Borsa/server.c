#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"


#define BUF_SIZE 1000


int port = 8000;


int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	
	LIST titoli, tmp;		
	itemType item, item2;

	int titoliLen, i;

	titoli = NewList();

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
	
   char buf[BUF_SIZE];	

	while(1) 
	{
		printf("Waiting for a new connection...\n");
		
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

		if(item.tipo == TIPO_AGENTE){
			printf("----- Gestione dell'agente di borsa -----\n");
			item.ricavo = 0;
			item.sockfd = newsockfd;
			titoli = EnqueueLast(titoli, item);
		}

		else if(item.tipo == TIPO_INVESTITORE){
			printf("----- Gestione dell'investitore -----\n");

			tmp = titoli;

			while(!isEmpty(tmp)){
				tmp->item.fine_lista = FALSE;
				printf("Invio del titolo:\n");
				PrintItem(tmp->item);
				if ( send( newsockfd, &tmp->item, sizeof(tmp->item), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
			}

			item.fine_lista = TRUE;
			if ( send( newsockfd, &item, sizeof(item), 0 ) == -1) 
			{
				perror("Error on send");
				exit(1);
			}

			if ( recv(sockfd, &item, sizeof(item), 0) == -1 ) 
			{
				perror("Error in receiving response from server\n");
				exit(1);
			}

			printf("Richiesto item:\n");
			PrintItem(item);
			close(newsockfd);

			printf("--- Aggiornamento dei titoli ---\n");
			titoliLen = getLength(titoli);

			for(i = 0; i < titoliLen; ++i){
				//legge il primo elemento e lo elimina dalla testa della lista
				item2 = getHead(titoli);
				titoli = DequeueFirst(titoli);
				printf("Letto il titolo:\n");
				PrintItem(item2);

				if(itemCompare(item2, item) == 0){
					item2.ricavo += item2.prezzo_unitario;
					item2.quantita--;
					item2.prezzo_unitario++; 
					
					printf("Aggiornato:\n");
					PrintItem(item2);
					

					if(item2.quantita > 0){	
						titoli = EnqueueLast(titoli, item2);
					}
					else{
						printf("Inviato il titolo all'agente\n");
						if ( send(item2.sockfd, &item2, sizeof(item2), 0) == -1 ) 
						{
							perror("Error in receiving response from server\n");
							exit(1);
						}
						
						close(item2.sockfd);
					}
				}
				else{
					item2.prezzo_unitario--;
					
					printf("Aggiornato:\n");
					PrintItem(item2);
					

					if(item2.prezzo_unitario >= item2.prezzo_minimo){	
						titoli = EnqueueLast(titoli, item2);
					}
					else{
						printf("Sotto il prezzo minimo. Termino la comunicazione con l'agente\n");
						if ( send(item2.sockfd, &item2, sizeof(item2), 0) == -1 ) 
						{
							perror("Error in receiving response from server\n");
							exit(1);
						}
					}
	
				}
			}
		}
		
		else{
			printf("Tipo messaggio non definito.\n");
			exit(1);
		}

		/* This sends the string plus the string terminator '\0' */
		/*
		if ( send( newsockfd, buf, len + 1, 0 ) == -1) 
		{
			perror("Error on send");
			exit(1);
		}
		*/

		printf("Titoli quotati:\n");
		PrintList(titoli);
	}
	
	titoli = DeleteList(titoli);

	close(sockfd);
	return 0;
}



