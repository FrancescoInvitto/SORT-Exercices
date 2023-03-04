#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"

int port = 8000;

/*
	Nome: Francesco
	Cognome: Invitto
	Matricola: 325559
*/

int main() 
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	LIST centri;
	LIST fornitori;
	LIST tmp, tmp1, tmp2;
	int quantita, q_tmp, trovato;

	itemType msg;

	itemType *centro;
	itemType fornitore;

	centri = NewList();
	fornitori = NewList();
	tmp = NewList();
	tmp2 = NewList();

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

		if(msg.tipo == TIPO_FORNITORE){
			printf("--- Gestione fornitore ---\n");

			printf("Controllo se sono presenti centri vaccinali che soddisfano il criterio per la distribuzione...\n");
			quantita = 0;
			q_tmp = msg.quantita;

			centro = Find(centri, msg);
			
			while(centro != NULL && msg.quantita > 0){
				quantita += centro->quantita;
				msg.quantita -= centro->quantita;

				tmp = EnqueueLast(tmp, *centro);
				centri = Dequeue(centri, *centro);

				if(msg.quantita != 0){
					centro = Find(centri, msg);
				}
			}

			if(quantita >= msg.min_quantita){
				printf("Trovati centri vaccinali che soddisfano la richiesta:\n");
				PrintList(tmp);

				printf("\nComunico al fornitore il numero di centri vaccinali che deve rifornire\n");

				int num_centri = getLength(tmp);
				if ( send( newsockfd, &num_centri, sizeof(int), 0 ) == -1) 
				{
					perror("Error on send");
					exit(1);
				}

				printf("Invio al fornitore la lista dei centri vaccinali che deve rifornire\n");

				while(!isEmpty(tmp)){
					if ( send( newsockfd, &tmp->item, sizeof(tmp->item), 0 ) == -1) 
					{
						perror("Error on send");
						exit(1);
					}

					printf("\nInvio il nome del fornitore al seguente centro vaccinale:\n");
					PrintItem(tmp->item);

					if ( send(tmp->item.sockfd, &msg, sizeof(msg), 0 ) == -1) 
					{
						perror("Error on send");
						exit(1);
					}
					
					close(tmp->item.sockfd);
					centri = Dequeue(centri, tmp->item);
					tmp = tmp->next;
				}

				printf("Lista inviata. Chiudo la connessione con il fornitore\n");
				close(newsockfd);
				
			}
			else{
				printf("Non ci sono centri vaccinali che soddisfino il criterio richiesto.\n");
				printf("Metto in attesa il fornitore.\n");
			
				msg.quantita = q_tmp;
				msg.sockfd = newsockfd;
				fornitori = EnqueueLast(fornitori, msg);

				while(!isEmpty(tmp)){
					centri = EnqueueOrdered(centri, tmp->item);
					tmp = tmp->next;
				}
				
			}
		}
		else if(msg.tipo == TIPO_CENTRO){
			printf("--- Gestione centro vaccinale ---\n");

			msg.sockfd = newsockfd;
			centri = EnqueueOrdered(centri, msg);

			printf("Controllo se è presente un fornitore in grado di soddisfare la domanda.\n");

			if(getLength(fornitori) == 0){
				printf("Nessun fornitore in attesa.\n");
			}
			else{
				trovato = 0;
				for(tmp2 = fornitori; tmp2 != NULL; tmp2 = tmp2->next){				
					fornitore = tmp2->item;

					quantita = 0;
					q_tmp = fornitore.quantita;

					centro = Find(centri, fornitore);
			
					while(centro != NULL && fornitore.quantita > 0){
						quantita += centro->quantita;
						fornitore.quantita -= centro->quantita;

						tmp = EnqueueLast(tmp, *centro);
						centri = Dequeue(centri, *centro);

						if(fornitore.quantita != 0){
							centro = Find(centri, fornitore);
						}
					}

					if(quantita >= fornitore.min_quantita){
						trovato = 1;
						fornitore.quantita = q_tmp;
						printf("Trovato fornitore in grado di soddisfare la richiesta:\n");
						PrintItem(fornitore);

						printf("\nCentri vaccinali che è in grado di servire:\n");
						PrintList(tmp);

						printf("Comunico al fornitore il numero di centri vaccinali che deve rifornire\n");

						int num_centri = getLength(tmp);
						if ( send( fornitore.sockfd, &num_centri, sizeof(int), 0 ) == -1) 
						{
							perror("Error on send");
							exit(1);
						}

						printf("Invio al fornitore la lista dei centri vaccinali che deve rifornire\n");

						while(!isEmpty(tmp)){
							if ( send( fornitore.sockfd, &tmp->item, sizeof(tmp->item), 0 ) == -1) 
							{
								perror("Error on send");
								exit(1);
							}
							
							printf("\nInvio il nome del fornitore al seguente centro vaccinale:\n");
							PrintItem(tmp->item);
							
							strncpy(msg.nome, fornitore.nome, LENMAX);

							if ( send(tmp->item.sockfd, &msg, sizeof(msg), 0 ) == -1) 
							{
								perror("Error on send");
								exit(1);
							}
						
							close(tmp->item.sockfd);
							centri = Dequeue(centri, tmp->item);

							tmp = tmp->next;
						}

						printf("Lista inviata. Chiudo la connessione con il fornitore\n");
						close(fornitore.sockfd);

						printf("Elimino il fornitore dalla lista.\n");
						fornitori = Dequeue(fornitori, tmp2->item);
				
					}
					else{
						/*printf("Il seguente fornitore non è in grado di soddisfare la richiesta\n");
						PrintItem(fornitore);*/
			
						tmp2->item.quantita = q_tmp;

						while(!isEmpty(tmp)){
							centri = EnqueueOrdered(centri, tmp->item);
							tmp = tmp->next;
						}
						//fornitori = EnqueueLast(fornitori, fornitore);	
					}
				}

				if(trovato == 0){
					printf("Nessun fornitore in grado di gestire la richiesta\n");
				}	
			}
		}
		else{
			printf("Attenzione! Tipo %d non definito!\n", msg.tipo);
			close(newsockfd);
		}

		/*
		if ( send( newsockfd, buf, len + 1, 0 ) == -1) 
		{
			perror("Error on send");
			exit(1);
		}
		*/
		printf("--- Fornitori in attesa ---\n");
		PrintList(fornitori);
		
		printf("--- Centri vaccinali in attesa ---\n");
		PrintList(centri);
	}

	centri = DeleteList(centri);
	fornitori = DeleteList(fornitori);

	close(sockfd);
	return 0;
}



