// 07700006172 Aishwarya Gupta Assignment 1 - server.c
// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "custom_packet.h"

#define PORT	 8080
#define MAXLINE sizeof(struct DataPacket)
#define MSG_CONFIRM 0
#define PACKET_MARKER 0XFFFF

// Function to handle errors and exit
void die(char *s) {
    perror(s);
    exit(1);
}

// Driver code
int main() {
    int serverSocket, clientSocket;
    char buffer[MAXLINE];
    socklen_t clientLength;
    // IPV4 socket address structure
    struct sockaddr_in serveraddr, clientaddr;
    struct ServerPacket packet;
    struct DataPacket datapacket;

    // Filling in packet, rest will be filled in while loop
    packet.start_of_packet = PACKET_MARKER;
    packet.end_of_packet = PACKET_MARKER;

    // Creating socket file descriptor
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    } else {
        printf("Socket Created \n");
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    memset(&clientaddr, 0, sizeof(clientaddr));

    // Filling server information
    serveraddr.sin_family = AF_INET; // IPv4
    serveraddr.sin_addr.s_addr = INADDR_ANY; // any internet address
    serveraddr.sin_port = htons(PORT); //16 bit big endian port (network byte order)

    // Bind the socket with the server address
    // serverSocket - file descriptor for a socket that will be bonded
    // pointer to a structure (struct sockaddr) that holds the local address information to which you want to bind the socket. 
    // the size of the addr structure in addrlen
    if (bind(serverSocket, (const struct sockaddr *)&serveraddr,
            sizeof(serveraddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    } else {
        printf("Bind Completed\n");
    }
    // allow 5 concurrent connections on socket
    listen(serverSocket, 5);

    int len, n;
    int received_packets = 0;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientaddr, &addr_size);

    while (1) {
        // Receive data packet from client
        clientLength = sizeof(clientaddr);
        n = recvfrom(serverSocket, &datapacket, sizeof(datapacket), 0, (struct sockaddr *)&clientaddr, &clientLength);
        if (n < 0) {
            die("Error receiving data");
        }
        printf("Received Data Packet (Sequence %d): %s\n", datapacket.segment_no, datapacket.payload);
        // Check sequence number and send ACK or Reject Packet
        if (datapacket.segment_no == received_packets){
            packet.client_id = datapacket.client_id;
            packet.segment_no = datapacket.segment_no;
            strcpy(packet.type, "RJ");
            packet.reject_code = 0XFFF7;
            sendto(serverSocket, &packet, sizeof(packet), 0, (struct sockaddr *)&clientaddr, clientLength);
            printf("Duplicate Packet.Sent REJECT\n");
            continue;
        // Check start and end markers
        } else if (datapacket.start_of_packet != PACKET_MARKER || datapacket.end_of_packet != PACKET_MARKER) {
            // Invalid packet, send Reject Packet
            packet.client_id = datapacket.client_id;
            packet.segment_no = datapacket.segment_no;
            strcpy(packet.type, "RJ");
            packet.reject_code = 0XFFF6;
            sendto(serverSocket, &packet, sizeof(packet), 0, (struct sockaddr *)&clientaddr, clientLength);
            printf("Start or end of packet missing. Sent REJECT\n");
            continue;
        } else if (datapacket.segment_no != received_packets && datapacket.segment_no != received_packets + 1) {
            // Invalid packet, send Reject Packet
            packet.client_id = datapacket.client_id;
            packet.segment_no = datapacket.segment_no;
            strcpy(packet.type, "RJ");
            packet.reject_code = 0XFFF4;
            sendto(serverSocket, &packet, sizeof(packet), 0, (struct sockaddr *)&clientaddr, clientLength);
            printf("Out of Sequence packet received. Sent REJECT\n");
            continue;
        } else if (datapacket.length != sizeof(struct DataPacket)) {
            // Invalid packet, send Reject Packet
            packet.client_id = datapacket.client_id;
            packet.segment_no = datapacket.segment_no;
            strcpy(packet.type, "RJ");
            packet.reject_code = 0XFFF4;
            sendto(serverSocket, &packet, sizeof(packet), 0, (struct sockaddr *)&clientaddr, clientLength);
            printf("Invalid Packet length. Sent REJECT\n");
            continue;
        } else {
            received_packets++;
            packet.client_id = datapacket.client_id;
            packet.segment_no = datapacket.segment_no;
            strcpy(packet.type, "AK");
            sendto(serverSocket, &packet, sizeof(packet), 0, (struct sockaddr *)&clientaddr, clientLength);
            printf("_______________________________________________\n");
            printf("|%u|%u|%s|%u|%d|%s|%u|\n", datapacket.start_of_packet, datapacket.client_id, datapacket.type, datapacket.segment_no, datapacket.length, datapacket.payload, datapacket.end_of_packet);
            printf("_______________________________________________\n");
            printf("Sent ACK\n");
        }
    }

    close(serverSocket);

    return 0;
}
// 07700006172 Aishwarya Gupta Assignment 1 - server.c
