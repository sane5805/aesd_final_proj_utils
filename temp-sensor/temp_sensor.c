
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <mqueue.h>

/* Macros */
#define MLX90614_TA 			(0x06) //RAM register
#define MLX90614_TOBJ1 		(0x07) //RAM register
#define MLX90614_TOBJ2 		(0x08) //RAM register

#define SOFT_RESET_CMD			(0xFE)	//Soft-Reset command to humidity sensor
#define HUMIDITY_CMD			(0xE5)	//Hold master mode for measuring humidity

#define TEMPERATURE_SLAVE_ADDRESS	(0x5A) //address of the MLX90614 temperature sensor
#define HUMIDITY_SLAVE_ADDRESS	(0x40) //address of the HTU21D humidity sensor
#define I2C_DEV_PATH 			("/dev/i2c-1")

#define SLEEP_DURATION 		(1000000) // for giving the delay of 1 second

/* Just in case if these were not defined */
#ifndef I2C_SMBUS_READ 
#define I2C_SMBUS_READ 1 
#endif 
#ifndef I2C_SMBUS_WRITE 
#define I2C_SMBUS_WRITE 0 
#endif

struct mq_attr attr;

typedef union i2c_smbus_data i2c_data;

int main()
{
    uint8_t buf[1];
    uint8_t buf1[3] = {0};
    uint16_t humidity_sensor_data = 0;
    double temperature, humidity;
    int rv;
    buf[0] = SOFT_RESET_CMD;
    char command;
    uint8_t soft_reset_flag = 1;
    mqd_t mqd;
    char sensor_buffer[sizeof(double) + sizeof(double)];
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(double) + sizeof(double);
    // open i2c bus
    int fdev = open(I2C_DEV_PATH, O_RDWR); 
    if (fdev < 0) 
    {
        fprintf(stderr, "Failed to open I2C interface %s Error: %s\n", I2C_DEV_PATH, strerror(errno));
        return -1;
    }
    
    //storing the address in slave_addr variable
    unsigned char temp_slave_addr = TEMPERATURE_SLAVE_ADDRESS; 
    unsigned char humidity_slave_addr = HUMIDITY_SLAVE_ADDRESS;
    // trying to read something from the device using SMBus READ request
    i2c_data data;

    mqd = mq_open("/sendmq", O_CREAT | O_RDWR, S_IRWXU, &attr);
    if(mqd == (mqd_t)-1)
    {
        printf("\n\rError in creating a message queue. Error: %s", strerror(errno));
    }
    
    while(1)
    {
        if (ioctl(fdev, I2C_SLAVE, temp_slave_addr) < 0) 
        {
            fprintf(stderr, "Failed to select I2C-based temperature slave device! Error: %s\n", strerror(errno));
            return -1;
        }

        // enable checksums control
        if (ioctl(fdev, I2C_PEC, 1) < 0) 
        {
            fprintf(stderr, "Failed to enable SMBus packet error checking, error: %s\n", strerror(errno));
            return -1;
        }

        command = MLX90614_TOBJ1; //setting the command as 0x07
    
        // build request structure
        struct i2c_smbus_ioctl_data sdat = 
        {
            .read_write = I2C_SMBUS_READ,
            .command = command,
            .size = I2C_SMBUS_WORD_DATA,
            .data = &data
        };
        // do actual request
	if (ioctl(fdev, I2C_SMBUS, &sdat) < 0) 
	{
       	fprintf(stderr, "Failed to perform I2C_SMBUS transaction, error: %s\n", strerror(errno));
        	return -1;
    	}
	
	bzero(sensor_buffer, sizeof(double) + sizeof(double));
	// fetching the temperature data from the sensor
	temperature = (double) data.word; 
	
	// converting the temperature in Celsius using the formula from datasheet
    	temperature = (temperature * 0.02) - 0.01;
    	temperature = temperature - 273.15;

        //logging the temperature
    	//printf("Temperature of the busbar = %04.2f\n", temperature);

    	usleep(SLEEP_DURATION); //delay of 1 second
    	
    	if (ioctl(fdev, I2C_SLAVE, humidity_slave_addr) < 0) 
        {
            fprintf(stderr, "Failed to select I2C-based humidty slave device! Error: %s\n", strerror(errno));
            return -1;
        }
        if(soft_reset_flag)
        {
            rv = write (fdev, buf, 1);
            if (rv < 0)
            {
                printf ("\n\rError in writing (Soft reset).");
            }
            usleep (17000);
            buf[0] = HUMIDITY_CMD;
            soft_reset_flag = 0;
        }
        
        rv = write (fdev, buf, 1);
        if (rv < 0)
        {
            printf ("\n\rError in writing command to humidity.");
            exit(1);
        }
        sleep(1);	//delay of 1 second before read operation
        rv = read (fdev, buf1, 3);
        if(rv < 0)
        {
            printf("\n\rError in reading. Error: %s", strerror(errno));
            exit(1);
        }
        else if(rv == 0)
        {
            printf("\n\rNo data was read.");
        }
        usleep (4000);
        humidity_sensor_data = buf1[0] << 8;
        humidity_sensor_data += buf[1];
        humidity_sensor_data &= ~0x003;
        
        humidity = (-6.0 + 125.0 / 65536 * (double) humidity_sensor_data);       
                
    	memcpy(sensor_buffer, &temperature, sizeof(double));
    	memcpy(sensor_buffer + sizeof(double), &humidity, sizeof(double));
    	if(mq_send(mqd, sensor_buffer, sizeof(double) + sizeof(double), 1) == -1)
    	{
    	    printf("\n\rError in sending data via message queue. Error: %s", strerror(errno));
    	}
    	sleep(1);
    }
}
