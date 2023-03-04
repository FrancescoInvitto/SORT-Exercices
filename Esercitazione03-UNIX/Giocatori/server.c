#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"

int port = 8000;

int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	LIST giocatori;
	LIST connessi;
	itemType msg, answer;
	int random, posizione, punti;

	itemType *g;
	itemType *giocatore;

	LIST tmp, tmp1;

	giocatori = NewList();
	connessi = NewList();

	srand(time(NULL));

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
		printf("In attesa di un nuovo giocatore...\n");
		
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
		
		if(Find(giocatori, msg) == NULL){
			printf("Giocatore %s non registrato. Creazione del profilo...\n", msg.nome);
			msg.num_partite = 0;
			msg.punteggio = 0;
			
			giocatori = EnqueueLast(giocatori, msg);
			printf("Profilo creato\n");
		}
		else{
			printf("Giocatore %s già registrato\n", msg.nome);
		}

		if(Find(connessi, msg) == NULL){
			msg.sockfd = newsockfd;
			msg.num_giocatore = getLength(connessi);
			printf("Metto in attesa il giocatore %s\n", msg.nome);
			connessi = EnqueueLast(connessi, msg);
		}
		else{
			printf("Giocatore %s già connesso tramite altro client. Stacco questa connessione\n", msg.nome);
			if ( send( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
			{
				perror("Error on send");
				exit(1);
			}

			close(newsockfd);
		}

		printf("--- Giocatori registrati ---\n");
		PrintList(giocatori);

		printf("--- Giocatori connessi ---\n");
		PrintList(connessi);

		if(getLength(connessi) < N){
			printf("Non ci sono ancora abbastanza giocatori per simulare la partita\n");	
		}
		else{
			printf("Raggiunto numero sufficiente di giocatori. Simulo la partita...\n");
			posizione = 0;
			punti = 3;

			while(!isEmpty(connessi)){
				tmp = connessi;

				random = rand() % N;

				while(!isEmpty(tmp) && tmp->item.num_giocatore != random){
					tmp = tmp->next;
				}

				if(!isEmpty(tmp)){
					printf("--- %d° classificato ---\n", posizione+1);
					PrintItem(tmp->item);
					printf("Assegno %d punti\n", punti);
					
					giocatore = Find(connessi, tmp->item);

					if(giocatore != NULL){
						printf("Aggiorno le statistiche del giocatore\n");

						g = Find(giocatori, *giocatore);

						if(g != NULL){
							g->num_partite++;
							g->punteggio += punti;
						}

						printf("Comunico al giocatore il suo posizionamento\n");
						connessi = Dequeue(connessi, *giocatore);

						answer.posizione = posizione+1;
						if ( send( giocatore->sockfd, &answer, sizeof(answer), 0 ) == -1) 
						{
							perror("Error on send");
							exit(1);
						}
					
						close(giocatore->sockfd);
						
					}

					if(punti > 0){
						punti--;
					}
			
					if(posizione < N){
						posizione++;
					}
					
				}
	
			}
			
			printf("Partita terminata\n");

			printf("--- Giocatori registrati---\n");
			PrintList(giocatori);

		}

	}

	close(sockfd);
	return 0;
}



