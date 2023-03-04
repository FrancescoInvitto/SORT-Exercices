all: server casa_editrice lettore


server: server.o list.o
	gcc -Wall -o $@ $^

casa_editrice: casa_editrice.o list.o
	gcc -Wall -o $@ $^

lettore: lettore.o list.o
	gcc -Wall -o $@ $^

%.o: %.c
	gcc -c $<

clean:
	rm -rf *.o server casa_editrice lettore *~

