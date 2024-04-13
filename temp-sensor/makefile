CROSS_COMPILE = arm-buildroot-linux-gnueabihf-

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror -ggdb
endif

ifeq ($(LDFLAGS),)
	LDFLAGS= -lrt
endif

all: temp_sensor
default: temp_sensor

temp_sensor: temp_sensor.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

temp_sensor.o: temp_sensor.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -f *.o temp_sensor

