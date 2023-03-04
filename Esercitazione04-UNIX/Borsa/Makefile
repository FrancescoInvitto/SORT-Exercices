all: server agente_borsa investitore

server: server.o list.o
	gcc -Wall -o $@ $^

agente_borsa: agente_borsa.o list.o
	gcc -Wall -o $@ $^

investitore: investitore.o list.o
	gcc -Wall -o $@ $^

%.o: %.c
	gcc -c $<

clean:
	rm -rf *.o server agente_borsa investitore *~

