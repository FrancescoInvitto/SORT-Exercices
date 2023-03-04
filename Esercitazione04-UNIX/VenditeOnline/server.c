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

	LIST offerte[N];
	LIST acquirenti;
	itemType msg;
	itemType* offerta;
	int i, num_offerte, accettata;		//accettata = 1 --> offerta del venditore non ancora presente per il prodotto indicato --> possibile inserimento nella lista

	LIST tmp;

	for(i = 0; i < N; i++){
		offerte[i] = NewList();
	}

	acquirenti = NewList();


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

		printf("Nuova connessione:\n");
		PrintItem(msg);

	    	if(msg.tipo == TIPO_VENDITORE){
			printf("--- Gestione venditore ---\n");

			offerta = Find(offerte[msg.prodotto], msg);
			if(offerta == NULL){
				accettata = 1;
				printf("Inserisco l'offerta nella lista %d e metto in attesa il venditore\n", msg.prodotto);
				msg.sockfd = newsockfd;
				offerte[msg.prodotto] = EnqueueOrdered(offerte[msg.prodotto], msg);
				//offerte[msg.prodotto] = EnqueueLast(offerte[msg.prodotto], msg);
				if ( send( newsockfd, &accettata, sizeof(int), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
			}
			else{
				accettata = 0;
				printf("Offerta del venditore %s già presente per il prodotto %d. Annullo la richiesta e termino la comunicazione.\n", msg.nome, msg.prodotto);
				if ( send( newsockfd, &accettata, sizeof(int), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
				close(newsockfd);
			}
		}
		else if(msg.tipo == TIPO_ACQUIRENTE){
			printf("--- Gestione acquirente ---\n");

			tmp = offerte[msg.prodotto];

			printf("Comunico al cliente quante sono le offerte per il prodotto %d\n", msg.prodotto);
			num_offerte = getLength(offerte[msg.prodotto]);
			
			if ( send( newsockfd, &num_offerte, sizeof(int), 0 ) == -1) 
			{
				perror("Error on send");
				exit(1);
			}

			printf("Invio al cliente la lista delle offerte relative al prodotto con codice: %d\n", msg.prodotto);
			while(!isEmpty(tmp)){
				
				if ( send( newsockfd, &tmp->item, sizeof(tmp->item), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}

				tmp = tmp->next;
			}
			printf("Lista inviata. Rimango in attesa di ricevere il nome del venditore...\n");
			
			if ( recv( newsockfd, &msg, sizeof(msg), 0 ) == -1) 
			{
				perror("Error on receive");
				exit(1);
			}

			printf("Ricevuto:\n");
			PrintItem(msg);
			printf("Termino la comunicazione con l'acquirente\n");
			close(newsockfd);

			offerta = Find(offerte[msg.prodotto], msg);

			if(offerta == NULL){
				printf("Non esiste nessuna offerta del venditore %s\n", msg.nome);
			}
			else{
				printf("Trovata l'offerta del venditore:\n");
				PrintItem(*offerta);
				printf("Aggiorno la disponibilità del prodotto\n");
				offerta->quantita--;
				printf("Sono rimaste %d unità del prodotto\n", offerta->quantita);
				
				if ( send( offerta->sockfd, offerta, sizeof(*offerta), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}
				
				if(offerta->quantita == 0){
					printf("Scorte del prodotto esaurite. Termino la comunicazione con il venditore %s\n", offerta->nome);
					close(offerta->sockfd);
					offerte[offerta->prodotto] = Dequeue(offerte[offerta->prodotto], *offerta);
				}
			}

				
			
		}
		else{
			printf("Tipo: %d non definito! Termino la comunicazione\n", msg.tipo);
			close(newsockfd);
		}
	
		printf("\n");		//scopo di facilità di lettura

		for(i = 0; i < N; i++){
			printf("--- Offerte prodotto %d ---\n", i);
			PrintList(offerte[i]);
			printf("\n");		//scopo di facilità di lettura
		}

	}

	for(i = 0; i < N; i++){
		offerte[i] = DeleteList(offerte[i]);
	}

	close(sockfd);
	return 0;
}



