all: server maestro nuotatore

server: server.o list.o
	gcc -Wall -o $@ $^

maestro: maestro.o list.o
	gcc -Wall -o $@ $^

nuotatore: nuotatore.o list.o
	gcc -Wall -o $@ $^

%.o: %.c
	gcc -c $<

clean:
	rm -rf *.o server maestro nuotatore *~

