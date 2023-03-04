all: server giocatore

server: server.o list.o
	gcc -Wall -o $@ $^

giocatore: giocatore.o list.o
	gcc -Wall -o $@ $^

%.o: %.c
	gcc -c $<

clean:
	rm -rf *.o server giocatore *~
