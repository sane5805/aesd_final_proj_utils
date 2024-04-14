CROSS_COMPILE =

CC=$(CROSS_COMPILE)gcc

CFLAGS= -g -Wall -Werror -ggdb

LDFLAGS= -lrt

all: temp_sensor
default: temp_sensor

temp_sensor : temp_sensor.o
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o temp_sensor temp_sensor.o $(LDFLAGS)

temp_sensor.o : temp_sensor.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c temp_sensor.c $(LDFLAGS)

clean :
	rm -f *.o temp_sensor

