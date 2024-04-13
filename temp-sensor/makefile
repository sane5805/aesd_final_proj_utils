CROSS_COMPILE = /home/sane5805/AESD/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-

CC=$(CROSS_COMPILE)gcc

CFLAGS= -g -Wall -Werror -ggdb

LDFLAGS= -lrt

all: temp_sensor
default: temp_sensor

temp_sensor: temp_sensor.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

temp_sensor.o: temp_sensor.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -f *.o temp_sensor

