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
	struct sockaddr_in serv_addr;
 	struct hostent* server;

	int orari[N];
	int i, num, j, trovato;
	itemType msg, answer;
	
	if (argc < 2){ 
		printf("Usage: %s [elenco orari]\n", argv[0]);
		return -1;
	}
	
	msg.pid = getpid();
	msg.tipo = TIPO_MAESTRO;
	msg.sockfd = -1;
	msg.num_ore = 0;
	msg.orario = -1;

	num = 1;
	
	for(i = 1; i < argc; i++){
		trovato = 0;
		if(msg.num_ore < 12){
			if(atoi(argv[i]) < 9 || atoi(argv[i]) > 20){
				printf("Orario %d non compreso nella fascia 9 - 20\n", atoi(argv[i]));
				num++;
			}
			else{
				for(j = 0; j < i-1; j++){
					if(atoi(argv[i]) == orari[j]){
						trovato = 1;		
					}
				}
				if(trovato == 0){
					msg.num_ore++;
					orari[i-1*num] = atoi(argv[i]);
				}
				else{
					num++;
					printf("Orario %d già indicato\n", atoi(argv[i]));
				}
			}
		}
		else{
			printf("Hai dato la disponibilità per tutti gli orari disponibili\n");
			break;
		}	
	}

	if(msg.num_ore == 0){
		printf("Non hai dato disponibilità per nessun orario!\n");
		return -1;
	}

	printf("--- Orari disponibilità ---\n");
	for(i = 0; i < msg.num_ore; i++){
		printf("%d\n", orari[i]);
	}
		
	
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


	printf("Mi collego al server e gli comunico per quante ore sono disponibile.\n");
	PrintItem(msg);

	if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("Inviato! In attesa di risposta dal server...\n");

	if ( recv(sockfd, &answer, sizeof(answer), 0) == -1 ) 
	{
		perror("Error in receiving response from server\n");
		exit(1);
	}

	if(answer.pid < 0){
		printf("Operazione fallita!\n");
	}
	else{
		if(msg.num_ore < 12){
			printf("Ok dal server. Invio tutti gli orari in cui sono disponibile\n");
			for(i = 0; i < msg.num_ore; i++){
				msg.orario = orari[i];
				if ( send(sockfd, &msg, sizeof(msg), 0) == -1 ) 
				{
					perror("Error on send\n");
					exit(1);
				}
			}
		}	
	}

	printf("Operazione completata!\n");

	close(sockfd);

	return 0;
}



