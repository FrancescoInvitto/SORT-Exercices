all: server client clab

server: server.o list.o
	gcc -Wall -o $@ $^

client: client.o list.o
	gcc -Wall -o $@ $^

clab: clab.o list.o
	gcc -Wall -o $@ $^

%.o: %.c
	gcc -c $<

clean:
	rm -rf *.o server client clab *~
