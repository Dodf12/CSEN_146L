//COEN 146L : Lab3, step 1: TCP client requesting a file transfer from the server.
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>

/*
* Name: Abhinav
* Date: 10/06/2025 (the day you have lab)
* Title: Lab3 - Part 1
* Description: 
*/
int main(int argc, char *argv[]){
    //Get from the command line, server IP, src and dst files.
    if (argc != 5){
		printf ("Usage: %s <ip of server> <port #> <src_file> <dest_file>\n",argv[0]);
		exit(0);
    } 

    printf("Hello1\n");
    //Declare socket file descriptor and buffer
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    char buf[10];


    const char *server_ip = argv[1];
    const char *port_str  = argv[2];
    const char *src_name  = argv[3];
    const char *dst_path  = argv[4];
        printf("Hello2\n");
    //Declare server address to accept
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port= htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    printf("Hello3\n");
   //Declare host
    

    //get hostname
    

    //Open a socket, if successful, returns
    int num;
    const struct sockaddr_in *temp = &server_addr;
    num = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("%d", num);
    printf("Hello4\n");
    //Set the server address to send using socket addressing structure
    

    //Connect to the server
    
    //Client sends the name of the file to retrieve from the server
    //printf("Hello5\n");
    write(sockfd, argv[3], strlen(argv[3]));
    printf("Hello5\n");
    int fileToWrite;
    printf("Hello6\n");
    fileToWrite = open(argv[4], O_WRONLY);

    printf("Hello7\n");
    //Client begins to write and read from the server
    ssize_t bytes_read;
    while((bytes_read = read(sockfd, buf, sizeof(buf))) > 0)
    {
      write(fileToWrite, buf, bytes_read);
    }
    //Close socket descriptor
    close(sockfd);
    return 0;
}

