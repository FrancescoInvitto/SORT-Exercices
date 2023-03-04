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

	LIST libri;
	LIST lettori;
	itemType msg;
	itemType* libro;
	itemType* lettore;
	int answer;

	//inizializziamo le liste come liste vuote

	libri = NewList();
	lettori = NewList();

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
		
		if ( recv(newsockfd, &msg, sizeof(msg), 0) == -1 ) 
		{
			perror("Error on receive\n");
			exit(1);
		}

		printf("Ricevuto il messaggio\n");
		PrintItem(msg);

		if(msg.tipo == TIPO_LETTORE){
			//lettore
			printf("----- Gestione del lettore -----\n");

			libro = Find(libri, msg);

			if(libro != NULL){
				printf("Libro presente nell'archivio: lo comunico al lettore\n");
				answer = 1;
				if ( send(newsockfd, &answer, sizeof(answer), 0) == -1 ) 
				{
					perror("Error on send\n");
					exit(1);
				}
				
				close(newsockfd);

				libro->num_copie --;
				if(libro->num_copie == 0){
					printf("Tutte le copie sono esaurite: eliminazione dal magazzino\n");
					libri = Dequeue(libri, *libro);
				}
			}
			else{
				printf("Il titolo richiesto non è in archivio: attendere...\n");
				msg.sockfd = newsockfd;
				lettori = EnqueueLast(lettori, msg);
			}			

			
		}
		else if (msg.tipo == TIPO_CASA_EDITRICE){
			//casa editrice
			printf("----- Gestione della casa editrice -----\n");
			answer = 1;
			if ( send(newsockfd, &answer, sizeof(answer), 0) == -1 ) 
				{
					perror("Error on send\n");
					exit(1);
				}	
			close(newsockfd);
			printf("Terminata la connessione con la casa editrice\n");
			
			lettore = Find(lettori, msg);
			while(msg.num_copie > 0 && lettore != NULL){
				printf("Trovato un lettore in attesa del libro:\n");
				PrintItem(*lettore);
				if ( send(lettore->sockfd, &answer, sizeof(answer), 0) == -1 ) 
				{
					perror("Error on send\n");
					exit(1);
				}
				close(lettore->sockfd);
				
				lettori = Dequeue(lettori, *lettore);
				
				msg.num_copie --;
				
				if(msg.num_copie > 0){
					lettore = Find(lettori, msg);
				}
			}

			if(msg.num_copie > 0){
				printf("Sono rimaste %d copie da mettere in archivio\n", msg.num_copie);
				libro = Find(libri, msg);
				if(libro == NULL){
					libri = EnqueueLast(libri, msg);
				}
				else{
					libro->num_copie += msg.num_copie;
				}	
			}
		}
		else{
			printf("Tipo di client non definito!");
			close(newsockfd);
		}

		printf("Lista libri in archivio:\n");
		PrintList(libri);
		printf("Lista lettori in attesa:\n");
		PrintList(lettori);

	}

	libri = DeleteList(libri);
	lettori = DeleteList(lettori);

	close(sockfd);
	return 0;
}



