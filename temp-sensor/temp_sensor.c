#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h> // Include POSIX message queue library
#include <linux/i2c-dev.h>

#define I2C_DEV_PATH            "/dev/i2c-1"

#define TEMPERATURE_SENSOR_ADDRESS  0x5A
#define OBJECT_TEMPERATURE_REGISTER 0x07

#define SLEEP_DURATION          (1000000)

typedef struct {
    double temperature;
} Message;

int fdev;
mqd_t mq;

void initialize() {
    fdev = open(I2C_DEV_PATH, O_RDWR);
    if (fdev < 0) {
        fprintf(stderr, "Failed to open I2C interface %s Error: %s\n", I2C_DEV_PATH, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Open or create the message queue
    mq = mq_open("/temperature_queue", O_CREAT | O_RDWR, 0666, NULL);
    if (mq == (mqd_t)-1) {
        fprintf(stderr, "Failed to open message queue: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void read_temperature() {
    unsigned char sensor_slave_address = TEMPERATURE_SENSOR_ADDRESS;

    while(1) {
        if (ioctl(fdev, I2C_SLAVE, sensor_slave_address) < 0) {
            fprintf(stderr, "Failed to select I2C slave device! Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Reading temperature from the sensor
        // Assume your existing code for reading the temperature here

        // Sending the temperature to the message queue
        Message msg;
        // Calculate temperature
        msg.temperature = 25.0; // Example value, replace this with actual temperature
        if (mq_send(mq, (const char *)&msg, sizeof(Message), 0) == -1) {
            fprintf(stderr, "Failed to send message to queue: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        usleep(SLEEP_DURATION);
    }
}

int main() {
    initialize();
    read_temperature();
    return 0;
}
