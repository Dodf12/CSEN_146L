// COEN 146L: Lab5, UDP client with checksum, seq, ack (fixed)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

// ---- Checksum (XOR of header+payload); use unsigned char to avoid sign issues ----
static int getChecksum(Packet packet) {
    packet.header.cksum = 0;  // must be zeroed for checksum calculation
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

// ---- Stop-and-wait send: transmit exactly Header+len bytes and await good ACK ----
static void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet) {
    for (;;) {
        // compute checksum and wire length
        packet.header.cksum = getChecksum(packet);
        size_t wire_len = sizeof(Header) + (size_t)packet.header.len;

        // send
        printf("Client sending packet\n");
        ssize_t sent = sendto(sockfd, &packet, wire_len, 0, address, addrlen);
        if (sent == -1) {
            perror("sendto");
            continue;
        }
        printf("Number of bytes sent: %zd\n", sent);

        // receive ACK
        Packet recvpacket = {0};
        socklen_t _tmp_len = addrlen;  // recvfrom may modify
        ssize_t r = recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0,
                             (struct sockaddr *)address, &_tmp_len);
        if (r == -1) {
            perror("recvfrom");
            continue;
        }
        printf("Received ACK bytes: %zd\n", r);
        printf("Client received ACK %d, checksum %d\n",
               recvpacket.header.seq_ack, recvpacket.header.cksum);

        // verify ACK checksum
        int expected_cksum = getChecksum(recvpacket);
        if (expected_cksum != recvpacket.header.cksum) {
            printf("Client: Bad checksum on ACK (expected %d)\n", expected_cksum);
            continue;
        }

        // verify ACK sequence (mirror our seq)
        if (packet.header.seq_ack != recvpacket.header.seq_ack) {
            printf("Client: Bad seqnum in ACK (expected %d)\n", packet.header.seq_ack);
            continue;
        }

        printf("Client: Good ACK\n");
        break;  // success
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <srcfile>\n", argv[0]);
        return 1;
    }

    // socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); return 1; }

    // server addr
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port   = htons((uint16_t)atoi(argv[2]));
    in_addr_t ip = inet_addr(argv[1]);
    if (ip == INADDR_NONE) {
        fprintf(stderr, "Invalid IP address: %s\n", argv[1]);
        close(sockfd);
        return 1;
    }
    servAddr.sin_addr.s_addr = ip;

    // open source file read-only
    int fp = open(argv[3], O_RDONLY);
    if (fp < 0) {
        perror("open srcfile");
        close(sockfd);
        return 1;
    }

    // send file contents
    socklen_t addr_len = sizeof(servAddr);
    Packet packet = {0};
    int seqNum = 0;  // start at 0, then toggle
    ssize_t n;

    while ((n = read(fp, packet.data, sizeof(packet.data))) > 0) {
        packet.header.len     = (int)n;
        packet.header.seq_ack = seqNum;
        printPacket("Client TX", packet);
        clientSend(sockfd, (struct sockaddr *)&servAddr, addr_len, packet);
        seqNum ^= 1;  // toggle 0<->1
        memset(&packet, 0, sizeof(packet)); // clear buffer so we don't leak old bytes into checksum
    }

    // send final zero-length packet
    Packet final = {0};
    final.header.len     = 0;
    final.header.seq_ack = seqNum;
    printPacket("Client TX (final)", final);
    clientSend(sockfd, (struct sockaddr *)&servAddr, addr_len, final);

    close(fp);
    close(sockfd);
    return 0;
}
