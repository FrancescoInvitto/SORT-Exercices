all: server automobilista

server: server.o list.o
	gcc -Wall -o $@ $^

automobilista: automobilista.o list.o
	gcc -Wall -o $@ $^

%.o: %.c
	gcc -c $<

clean:
	rm -rf *.o server automobilista *~
