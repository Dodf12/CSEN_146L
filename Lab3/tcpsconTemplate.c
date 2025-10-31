// COEN 146L : Lab3, step 3: concurrent TCP server that accepts and responds to multiple client connection requests, each requesting a file transfer
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>


/*
* Name: Abhinav
* Date: 10/10/2023 (the day you have lab)
* Title: Lab3 - Part 1
* Description: This program implements a TCP server that accepts a client connection for file transfer. Ifmultiple files are 
sent by different clients, it handles each connection in another thread
*/

// Define the number of clients/threads that can be served
#define N 100
int threadCount = 0;
pthread_t clients[N]; // declaring N threads

// Declare socket and connection file descriptors.
int sockfd, connfd;
// Declare receiving and sending buffers of size 10 bytes
char receiving[10];
char sending[10];

// Declare server address to which to bind for receiving messages and client address to fill in sending address
struct sockaddr_in serverAddr;
socklen_t addr_size;

// Connection handler (thread function) for servicing client requests for file transfer
void *connectionHandler(void *sock)
{
   int sockInt = *(int *)sock;
   free(sock); // Free the allocated memory for socket descriptor

   // declare buffer holding the name of the file from client
   char fname[256];
   char buffer[10];

   // Connection established, server begins to read and write to the connecting client
   printf("Connection Established with client\n");

   // receive name of the file from the client
   int n = recv(sockInt, fname, sizeof(fname) - 1, 0);
   if (n <= 0)
   {
      close(sockInt);
      pthread_exit(0);
   }
   fname[n] = '\0';

   // open file and send to client
   FILE *myFile = fopen(fname, "rb");
   if (!myFile)
   {
      perror("fopen");
      close(sockInt);
      pthread_exit(0);
   }

   printf("Hello! 8\n");

   // read file and send to connection descriptor
   size_t bytes_read;
   while ((bytes_read = fread(buffer, 1, sizeof(buffer), myFile)) > 0)
   {
      size_t off = 0;
      while (off < bytes_read)
      {
         ssize_t s = send(sockInt, buffer + off, bytes_read - off, 0);
         if (s < 0)
         {
            perror("send");
            break;
         }
         off += (size_t)s;
      }
   }

   printf("File transfer complete\n");

   // close file and connection descriptor
   fclose(myFile);
   close(sockInt);

   pthread_exit(0);
}

int main(int argc, char *argv[])
{
   // Get from the command line, server IP, src and dst files.
   if (argc != 2)
   {
      printf("Usage: %s <port #> \n", argv[0]);
      exit(0);
   }
   // Open a TCP socket, if successful, returns a descriptor
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   // Setup the server address to bind using socket addressing structure
   // struct sockaddr_in serverAddr;
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_port = htons(atoi(argv[1]));
   serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

   // bind IP address and port for server endpoint socket
   if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
   {
      perror("bind");
      close(sockfd);
      return 1;
   }

   // Server listening to the socket endpoint, and can queue 5 client requests
   if (listen(sockfd, 5) < 0)
   {
      perror("listen");
      close(sockfd);
      return 1;
   }
   printf("Server listening/waiting for client at port %s\n", argv[1]);

   while (1)
   {
      // Server accepts the connection and call the connection handler
      struct sockaddr_in clientAddr;
      socklen_t clientLen = sizeof(clientAddr);
      int new_connfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientLen);
      if (new_connfd < 0)
      {
         perror("accept");
         continue;
      }

      sleep(5);

      // Allocate memory for socket descriptor to avoid race condition
      int *sock_ptr = malloc(sizeof(int));
      *sock_ptr = new_connfd;

      if (pthread_create(&clients[threadCount], NULL, connectionHandler, (void *)sock_ptr) < 0)
      {
         perror("Unable to create a thread");
         close(new_connfd);
         free(sock_ptr);
         continue;
      }
      else
         printf("Thread %d has been created to service client request\n", ++threadCount);
   }
   for (int i = 0; i < threadCount; i++)
   {
      pthread_join(clients[i], NULL);
   }

   pthread_exit(NULL);
   return 0;
}