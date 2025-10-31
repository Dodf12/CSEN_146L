// COEN 146L : Lab4, step 2: UDP client (peer) that sends a file to a server (peer)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

// Declare a Header structure that holds length of a packet
typedef struct
{
    int len;
} Header;

// Declare a packet structure that holds data and header
typedef struct
{
    Header header;
    char data[10];
} Packet;

// Printing received packet
void printPacket(Packet packet)
{
    printf("packet sent\n");
}

void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet *packet)
{
    while (1)
    {
        // send the packet
        printf("Client sending packet\n");
        if (sendto(sockfd, packet, sizeof(Packet), 0, address, addrlen) < 0)
        {
            perror("sendto");
            return;
        }

        // receive an ACK from the server
        Packet recvpacket;
        ssize_t n = recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, NULL, NULL);
        if (n < 0)
        {
            perror("recvfrom");
            return;
        }

        // print received packet
        printPacket(recvpacket);
        break;
    }
}

int main(int argc, char *argv[])
{
    // Get from the command line, server IP, Port and src file
    if (argc != 4)
    {
        printf("Usage: %s <ip> <port> <srcfile>\n", argv[0]);
        exit(0);
    }

    // Declare socket file descriptor and open UDP socket
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    // Set the server address to send using socket addressing structure
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[2]));
    if (inet_aton(argv[1], &servAddr.sin_addr) == 0)
    {
        fprintf(stderr, "Invalid IP address: %s\n", argv[1]);
        close(sockfd);
        exit(1);
    }

    // Open file given by argv[3]
    int fp = open(argv[3], O_RDWR);
    if (fp < 0)
    {
        perror("Failed to open file\n");
        exit(1);
    }

    // send file contents to server
    socklen_t addr_len = sizeof(servAddr);
    Packet packet;
    ssize_t nread;
    while ((nread = read(fp, packet.data, sizeof(packet.data))) > 0)
    {
        packet.header.len = (int)nread;
        // send packet
        clientSend(sockfd, (struct sockaddr *)&servAddr, addr_len, &packet);
    }
    if (nread < 0)
    {
        perror("read");
    }

    // sending zero-length packet (final) to server to end connection
    Packet final;
    final.header.len = 0;
    clientSend(sockfd, (struct sockaddr *)&servAddr, addr_len, &final);

    // close file and socket
    close(fp);
    close(sockfd);
    return 0;
}
