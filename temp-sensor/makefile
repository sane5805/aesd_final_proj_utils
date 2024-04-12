CC=gcc
CFLAGS=-g -Wall -Werror
LDFLAGS=-lrt

server: temp_sensor.o
	$(CC) $(CFLAGS) -o temp_sensor temp_sensor.o $(LDFLAGS)

server.o: temp_sensor.c
	$(CC) $(CFLAGS) -c temp_sensor.c $(LDFLAGS)

clean:
	rm -f temp_sensor *.o
