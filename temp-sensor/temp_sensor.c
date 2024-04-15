#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c.h> // Include this for I2C_SMBUS_READ and I2C_SMBUS_WORD_DATA
// Assuming i2c_smbus_data is defined in linux/i2c-dev.h, but it might be in another header.
#include <linux/i2c-dev.h> 
#include <mqueue.h> // Include POSIX message queue library
#include <sys/socket.h>
#include <netinet/in.h>


#define I2C_DEV_PATH			"/dev/i2c-1"

// Define constants for temperature sensor and I2C communication
#define TEMPERATURE_SENSOR_ADDRESS	0x5A
#define TEMPERATURE_REGISTER		0x06
#define OBJECT_TEMPERATURE_REGISTER	0x07

#define SLEEP_DURATION		(1000000)

typedef struct {
    double temperature;
} Message;

// Define union for I2C data
typedef union i2c_smbus_data i2c_data;
// Declare file descriptor for I2C device
int fdev;

mqd_t mq;

// Function to initialize I2C communication and message queue
void initialize() {
    
    // Open I2C device
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

// Function to continuously read temperature from the sensor and send it to the message queue
void read_temperature() {
    
    // Set soft reset command and sensor address
    unsigned char sensor_slave_address = TEMPERATURE_SENSOR_ADDRESS;
    
    while(1) {
        
        // set slave device address, default MLX is 0x5A
        if (ioctl(fdev, I2C_SLAVE, sensor_slave_address) < 0) {
            fprintf(stderr, "Failed to select I2C slave device! Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        // enable checksums control
        if (ioctl(fdev, I2C_PEC, 1) < 0) {
            fprintf(stderr, "Failed to enable SMBus packet error checking, error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
	    
        i2c_data data;
        
        // Set command to read object temperature
        char command = OBJECT_TEMPERATURE_REGISTER;
        
        // build request structure
        struct i2c_smbus_ioctl_data sdata = {
            .read_write = I2C_SMBUS_READ,
            .command = command,
            .size = I2C_SMBUS_WORD_DATA,
            .data = &data
        };
        
        // do actual request
        if (ioctl(fdev, I2C_SMBUS, &sdata) < 0) {
            fprintf(stderr, "Failed to perform I2C_SMBUS transaction, error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        // calculate temperature in Celsius by formula from datasheet
	    double temp = (double) data.word;
        temp = (temp * 0.02) - 0.01;
     	temp = temp - 273.15;
	    
    	// print result
    	//fprintf(stdout, "\n temp = %f \n", temp);

        // Sending the temperature to the message queue
        Message msg;
        // Calculate temperature
        msg.temperature = temp; // Example value, replace this with actual temperature
        if (mq_send(mq, (const char *)&msg, sizeof(Message), 0) == -1) {
            fprintf(stderr, "Failed to send message to queue: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
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
