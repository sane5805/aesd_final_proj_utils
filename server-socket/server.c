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
   
   
mqd_t mqd;
int sockfd, connfd;
struct mq_attr attr;

// Function designed for chat between client and server. 
void func(int connfd) 
{ 
    char buff[sizeof(double)];
    double temp;
    char temp_value_for_client[60]; 
    unsigned int msg_prio;

    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, sizeof(double)); 
   
        if (mq_receive(mqd, buff, sizeof(double), &msg_prio) == -1) {
            fprintf(stderr, "Failed to receive message from queue: %s\n", strerror(errno));
        } 
        // read the message from client and copy it in buffer 

        memcpy(&temp, buff, sizeof(double));
        //read(connfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents 

        sprintf(temp_value_for_client, "temp = %0.2lf", temp);
	
	// and send that buffer to client
        send(connfd, temp_value_for_client, strlen(toClient) + 1, 0);
   
   
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
    int len; 
    struct sockaddr_in servaddr, cli; 
   
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1}, sizeof(int)) == -1)
    {
        printf("\n\rError in setting up socket options. Error: %s", strerror(errno)); 
        exit(0); 
    }
    
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
    else {
        printf("Server listening..\n"); 
    	len = sizeof(cli); 
    }

    // message queue opening for retriving data from temperature sensor
    mqd = mq_open("/temperature_queue", O_RDWR);
    if (mqd == -1) {
        fprintf(stderr, "Failed to open the queue: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
   
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, (socklen_t *)&len); 
    if (connfd < 0) { 
        printf("server accept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server accept the client...\n"); 
   
    // Function for chatting between client and server 
    func(connfd); 
   
    // After chatting close the socket 
    close(sockfd); 
    close(connfd); 

    // Close message queue
    mq_close(mqd);
    mq_unlink("/temperature_queue");
}
