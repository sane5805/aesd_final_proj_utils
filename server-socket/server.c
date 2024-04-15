/************************************************************************************
 * @name: server.c
 * @brief: A socket program for a server in stream mode.
 * @author: Saurav Negi
 * @reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c
 ***********************************************************************************/

#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#include <mqueue.h> // Include POSIX message queue library
#include <fcntl.h>
#include <errno.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

typedef struct {
    double temperature;
} Message;
   
// Function designed for chat between client and server. 
void func(int sockfd, mqd_t mq) 
{ 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        // Receive message from message queue (Temperature from sensor)
        Message msg;
        if (mq_receive(mq, (char *)&msg, sizeof(Message), NULL) == -1) {
            if (errno != EAGAIN) {
                fprintf(stderr, "Failed to receive message from queue: %s\n", strerror(errno));
            }
        } else {
            printf("Received temperature from sensor: %f\n", msg.temperature);
            // Convert temperature to string and broadcast it to client
            char temperature_str[MAX];
            snprintf(temperature_str, MAX, "%f", msg.temperature);
            send(sockfd, temperature_str, strlen(temperature_str), 0);
        }

        // Introduce some delay before broadcasting the next temperature
        usleep(1000000); // Adjust as needed

        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
} 
   
// Driver function 
int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
    mqd_t mq;

    // Create message queue for communication between temp_sensor and server
    mq = mq_open("/temperature_queue", O_CREAT | O_RDWR, 0666, NULL);
    if (mq == (mqd_t)-1) {
        fprintf(stderr, "Failed to open message queue: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
   
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
   
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
   
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
   
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
   
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, (socklen_t *)&len); 
    if (connfd < 0) { 
        printf("server accept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server accept the client...\n"); 
   
    // Function for chatting between client and server 
    func(connfd, mq); 
   
    // After chatting close the socket 
    close(sockfd); 

    // Close message queue
    mq_close(mq);
    mq_unlink("/temperature_queue");
}
