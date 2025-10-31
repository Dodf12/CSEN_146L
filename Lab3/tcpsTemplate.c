// COEN 146L : Lab3, step 1: TCP server that accepts a client connection for file transfer.
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

/*
* Name: Abhinav
* Date: 10/10/2023 (the day you have lab)
* Title: Lab3 - Part 1
* Description: This program implements a TCP server that accepts a client connection for file transfer.
*/

// Declare socket file descriptor.
int sockfd;

// Declare receiving and sending buffers of size 10 bytes
char receiving[10];
char sending[10];

// Declare server address to which to bind for receiving messages and client address to fill in sending address
struct sockaddr_in server_address;
// int bind;

// Connection handler for servicing client request for file transfer
void *connectionHandler(void *sock)
{
  printf("Hello! 4\n");

  // declate buffer holding the name of the file from client
  // char fname[10];
  printf("Hello! 5\n");
  // get the connection descriptor
  int sockInt = *(int *)sock;
  printf("Hello! 6\n");
  // Connection established, server begins to read and write to the connecting client
  // printf("Connection Established with client IP: %s and Port: %d\n");
  // printf("Hello! 7\n");
  printf("Connection Established with client IP: %s and Port: %d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

  // receive name of the file from the client
  char fname[256];
  ssize_t n = recv(sockInt, fname, sizeof(fname) - 1, 0);
  if (n <= 0)
  {
    close(sockInt);
    return 0;
  }
  fname[n] = '\0';
  // open file and send to client
  FILE *myFile;
  myFile = fopen(fname, "r");
  printf("Hello! 8\n");
  // read file and send to connection descripton

  size_t bytes_read;
  while ((bytes_read = fread(receiving, 1, sizeof(receiving), myFile)) > 0)
  {
    size_t off = 0;
    while (off < bytes_read)
    {
      ssize_t s = send(sockInt, receiving + off, bytes_read - off, 0);
      off += (size_t)s;
    }
  }
  printf("Hello! 9\n");
  printf("File transfer complete\n");

  // close file
  fclose(myFile);

  // Close connection descriptor
  close(sockInt);
  return 0;
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
  printf("Hello! 1\n");
  // Setup the server address to bind using socket addressing structure
  server_address.sin_family = AF_INET;
  printf("Hello! 2\n");
  // bind IP address and port for server endpoint socket
  server_address.sin_port = htons(8080);
  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
  printf("Hello! 3\n");
  // Server listening to the socket endpoint, and can queue 5 client requests
  listen(sockfd, 5);
  printf("Server listening/waiting for client at port 8080\n");
  // Server accepts the connection and call the connection handler

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
  connectionHandler(&connfd);
  printf("Hello! 5\n");
  // close socket descriptor
  close(sockfd);
  return 0;
}