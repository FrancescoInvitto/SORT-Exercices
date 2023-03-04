all: server utente impiegato

server:	server.o list.o
	gcc -Wall -o server server.o list.o

utente:	utente.o
	gcc -Wall -o utente utente.o list.o

impiegato: impiegato.o
	gcc -Wall -o impiegato impiegato.o list.o

%.o: %.c 
	gcc -c $<

clean:
	rm -f server utente impiegato *.o *~

