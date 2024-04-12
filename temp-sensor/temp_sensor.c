#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>


// Define constants for temperature sensor and I2C communication
#define TEMPERATURE_REGISTER	(0x06)
#define OBJECT_TEMPERATURE_REGISTER	(0x07)
#define AMBIENT_TEMPERATURE_REGISTER	(0x08)

#define SOFT_RESET_COMMAND	(0xFE)
#define TEMPERATURE_SENSOR_ADDRESS	(0x5A)
#define I2C_DEVICE_PATH		("/dev/i2c-1")

#define SLEEP_DURATION		(1000000)

// Define message queue attributes
struct mq_attr message_queue_attributes;

// Define union for I2C data
typedef union i2c_smbus_data i2c_data;

// Declare message queue descriptor and file descriptor for I2C device
mqd_t message_queue_descriptor;
int file_descriptor;

// Function to initialize I2C communication and message queue
void initialize() {

    // Open I2C device
    file_descriptor = open(I2C_DEVICE_PATH, O_RDWR);
}

// Function to continuously read temperature from the sensor and send it to the message queue
void read_temperature() {
    uint8_t buffer[1];
    char command;
    i2c_data sensor_data;
    double temperature;

    // Set soft reset command and sensor address
    buffer[0] = SOFT_RESET_COMMAND;
    unsigned char sensor_slave_address = TEMPERATURE_SENSOR_ADDRESS;
    
    while(1) {
        // Select temperature sensor device on the I2C bus
        if (ioctl(file_descriptor, I2C_SLAVE, sensor_slave_address) < 0) {
            fprintf(stderr, "Failed to select I2C-based temperature sensor device! Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Enable SMBus packet error checking
        if (ioctl(file_descriptor, I2C_PEC, 1) < 0) {
            fprintf(stderr, "Failed to enable SMBus packet error checking, error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Set command to read object temperature
        command = OBJECT_TEMPERATURE_REGISTER;

        // Define transaction data structure for SMBus read
        struct i2c_smbus_ioctl_data transaction_data = {
            .read_write = I2C_SMBUS_READ,
            .command = command,
            .size = I2C_SMBUS_WORD_DATA,
            .data = &sensor_data
        };

        // Perform SMBus transaction to read temperature data
        if (ioctl(file_descriptor, I2C_SMBUS, &transaction_data) < 0) {
            fprintf(stderr, "Failed to perform I2C_SMBUS transaction, error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

	bzero(temp_val, sizeof(double) + sizeof(double));

        temp_val = ((temp_val * 0.02) - 0.01) - 273.15;
 	
	print("\n temp_val = %d \n");

        // Introduce delay
        usleep(SLEEP_DURATION);
    }
}

int main() {
    // Initialize I2C communication and message queue
    initialize();

    // Read temperature continuously
    read_temperature();

    return 0;
}
