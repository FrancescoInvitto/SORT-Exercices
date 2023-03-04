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

	LIST giorno[N];
	itemType msg;
	itemType* maestro;
	int i;

	for(i = 0; i < N; i++){
		giorno[i] = NewList();
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

		printf("Ricevuto:\n");
		PrintItem(msg);

	   	if(msg.tipo == TIPO_MAESTRO){
			printf("--- Gestione maestro ---\n");

			if(msg.num_ore == 12){
				printf("Il maestro con PID %d ha dato disponibilità per tutte le fasce orarie.\n", msg.pid);

				for(i = 0; i < N; i++){		
					msg.orario = i + 9;
					giorno[i] = EnqueueLast(giorno[i], msg);
				}

				if ( send( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}

			}
			else{
				printf("Il maestro con PID %d ha dato la disponibilità per %d fasce orarie. Gli do l'OK e me le faccio inviare...\n", msg.pid, msg.num_ore);

				if ( send( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}

				for(i = 0; i < msg.num_ore; i++){
					if ( recv( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
					{
						perror("Error on receive");
						exit(1);
					}
				
					printf("Orario ricevuto: %d\n", msg.orario);
					giorno[msg.orario - 9] = EnqueueLast(giorno[msg.orario - 9], msg);
				}
			}

			printf("Chiudo la connessione con il maestro\n");
			close(newsockfd);
		}
		else if(msg.tipo == TIPO_NUOTATORE){
			printf("--- Gestione nuotatore ---\n");

			printf("\nControllo la presenza di un maestro disponibile per l'orario %d...\n", msg.orario);

			maestro = Find(giorno[msg.orario - 9], msg);
		
			if(maestro == NULL){
				printf("Nessun maestro disponibile per l'orario indicato!\n");
				msg.pid = -1;				
			}
			else{
				printf("Maestro disponibile trovato:\n");
				PrintItem(*maestro);
				msg.pid = maestro->pid;
				printf("Rimuovo il maestro dalla lista corrispondente all'orario %d\n", maestro->orario);

				giorno[(maestro->orario) - 9] = Dequeue(giorno[(maestro->orario) - 9], *maestro);
			}

			if ( send( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
			{
				perror("Error on send");
				exit(1);
			}
			
			close(newsockfd);

			
		}
		else{
			printf("Tipo %d non valido! Chiudo la connessione\n", msg.tipo);
			close(newsockfd);
		}

		printf("----- Disponibilità -----\n");
		for(i = 0; i < N; i++){
			printf("-- Ore %d --\n", i + 9);
			PrintList(giorno[i]);
			printf("\n");
		}
	}

	for(i = 0; i < N; i++){
		giorno[i] = DeleteList(giorno[i]);
	}

	close(sockfd);
	return 0;
}



