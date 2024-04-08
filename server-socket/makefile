CC=gcc
CFLAGS=-g -Wall -Werror
LDFLAGS=-lrt

server: server.o
    $(CC) $(CFLAGS) -o server server.o $(LDFLAGS)

server.o: server.c
    $(CC) $(CFLAGS) -c server.c $(LDFLAGS)

clean:
    rm -f server *.o
