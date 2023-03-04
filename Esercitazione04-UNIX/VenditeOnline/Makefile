all: server venditore acquirente

server: server.o list.o
	gcc -Wall -o $@ $^

venditore: venditore.o list.o
	gcc -Wall -o $@ $^

acquirente: acquirente.o list.o
	gcc -Wall -o $@ $^

%.o: %.c
	gcc -c $<

clean:
	rm -rf *.o server venditore acquirente *~
