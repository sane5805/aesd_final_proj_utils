CROSS_COMPILE =

CC=$(CROSS_COMPILE)gcc

CFLAGS= -g -Wall -Werror -ggdb

LDFLAGS= -lrt

all: server
default: server

server : server.o
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o server server.o $(LDFLAGS)

server.o : server.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c server.c $(LDFLAGS)

clean :
	rm -f *.o server
