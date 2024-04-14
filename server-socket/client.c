/************************************************************************************
 * @name: client.c
 * @brief: A socket program for a client in stream mode.
 * @author: [Your Name]
 * @reference: https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application
 ***********************************************************************************/

#include <stdio.h>
#include <winsock2.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function to communicate with the server
void communicate_with_server(SOCKET sockfd)
{
    char buff[MAX];
    int n;

    for (;;)
    {
        printf("Enter the message: ");
        fgets(buff, MAX, stdin);
        send(sockfd, buff, strlen(buff), 0);
        if (strncmp(buff, "exit", 4) == 0)
        {
            printf("Client Exit...\n");
            break;
        }

        n = recv(sockfd, buff, sizeof(buff), 0);
        if (n <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }
        buff[n] = '\0';
        printf("From Server: %s\n", buff);
    }
}

int main()
{
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in servaddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed.\n");
        return 1;
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("socket creation failed...\n");
        WSACleanup();
        return 1;
    }
    else
        printf("Socket successfully created..\n");

    memset(&servaddr, 0, sizeof(servaddr));

    // Assign IP and PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("169.254.187.42"); // IP address of Raspberry Pi
    servaddr.sin_port = htons(PORT);

    // Connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    else
        printf("connected to the server..\n");

    // Function for communication with server
    communicate_with_server(sockfd);

    // Close the socket
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
