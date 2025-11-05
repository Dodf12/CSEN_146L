// COEN 146L: Lab5, UDP server with checksum, seq, ack (fixed)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>

// ---- Protocol types ----
typedef struct {
    int seq_ack;
    int len;
    int cksum;
} Header;

typedef struct {
    Header header;
    char data[10];
} Packet;

// ---- Globals (only for convenience) ----
static struct sockaddr_in servAddr, clienAddr;

// ---- Checksum ----
static int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    unsigned char *ptr = (unsigned char *)&packet;
    unsigned char *end = ptr + sizeof(Header) + packet.header.len;
    unsigned char checksum = 0;
    while (ptr < end) checksum ^= *ptr++;
    return (int)checksum;
}

static void printPacket(const char *tag, Packet packet) {
    printf("%s Packet{ header:{ seq_ack:%d len:%d cksum:%d }, data:\"",
           tag, packet.header.seq_ack, packet.header.len, packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

// ---- Send ACK: len=0, zero-initialized, send exactly Header bytes ----
static void serverSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    Packet packet = (Packet){0};
    packet.header.seq_ack = seqnum;
    packet.header.len     = 0;                     // ACK carries no data
    packet.header.cksum   = getChecksum(packet);

    size_t wire_len = sizeof(Header);
    ssize_t s = sendto(sockfd, &packet, wire_len, 0, address, addrlen);
    if (s == -1) perror("sendto");

    printf("\tServer sending ACK %d, checksum %d, bytes %zd\n",
           packet.header.seq_ack, packet.header.cksum, s);
}

// ---- Receive one correct packet for expected seqnum ----
static Packet serverReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet = {0};

    for (;;) {
        ssize_t r = recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        if (r == -1) {
            perror("recvfrom");
            continue;
        }
        printf("Number of bytes received: %zd\n", r);
        printPacket("Server RX", packet);

        int expected_cksum = getChecksum(packet);

        if (packet.header.cksum != expected_cksum) {
            printf("\tServer: Bad checksum (expected %d)\n", expected_cksum);
            serverSend(sockfd, address, *addrlen, !seqnum);  // NAK with opposite seq
            memset(&packet, 0, sizeof(packet));
            continue;
        } else if (packet.header.seq_ack != seqnum) {
            printf("\tServer: Bad seqnum (expected %d)\n", seqnum);
            serverSend(sockfd, address, *addrlen, !seqnum);  // resend last ACK
            memset(&packet, 0, sizeof(packet));
            continue;
        } else {
            printf("\tServer: Good packet\n");
            serverSend(sockfd, address, *addrlen, seqnum);   // ACK with expected seq
            break;
        }
    }
    return packet;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <port> <dstfile>\n", argv[0]);
        return 1;
    }

    // socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // bind
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    servAddr.sin_port        = htons((uint16_t)atoi(argv[1]));
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&servAddr, (socklen_t)sizeof(servAddr)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    // open/overwrite destination file
    int fp = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fp < 0) {
        perror("open dstfile");
        close(sockfd);
        return 1;
    }

    // receive loop
    int seqNum = 0;
    Packet packet;
    socklen_t addr_len = (socklen_t)sizeof(clienAddr);

    printf("Waiting for packets...\n");
    do {
        packet = serverReceive(sockfd, (struct sockaddr *)&clienAddr, &addr_len, seqNum);
        if (packet.header.len > 0) {
            ssize_t w = write(fp, packet.data, (size_t)packet.header.len);
        }
        seqNum ^= 1;  // toggle after accepting a good packet
    } while (packet.header.len != 0);  // zero-length packet signals end

    close(fp);
    close(sockfd);
    return 0;
}
