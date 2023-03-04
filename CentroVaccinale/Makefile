all: server centro fornitore

server: server.o list.o
	gcc -Wall -o $@ $^

centro: centro.o list.o
	gcc -Wall -o $@ $^

fornitore: fornitore.o list.o
	gcc -Wall -o $@ $^ 

%.o: %.c
	gcc -c $<

clean:
	rm -rf *.o server centro fornitore *~
