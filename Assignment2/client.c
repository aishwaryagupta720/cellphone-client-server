// 07700006172 Aishwarya Gupta Assignment 2 - client.c
// Implementation of client for UDP 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "custom_packet.h"
#include <sys/time.h>

// Define constants
#define PORT	     8888
#define MAXLINE      sizeof(struct PermissionPacket)
#define MSG_CONFIRM  0
#define MAX_RETRIES  2

// Function to handle errors and exit
void die(char *s) {
    perror(s);
    exit(1);
}

// Function to send a packet to the server
void sendPacket(int clientSocket, struct sockaddr_in serverAddr, struct PermissionPacket packet) {
    socklen_t serverLength = sizeof(serverAddr);
    int n = sendto(clientSocket, &packet, sizeof(struct PermissionPacket), 0, (struct sockaddr *)&serverAddr, serverLength);
    if (n < 0) {
        die("Error sending data");
    }
}

// Function to receive a packet with a timeout
int receivePacketWithTimeout(int clientSocket, struct sockaddr_in serverAddr, struct PaymentPacket *packet, int timeoutSeconds) {
    // Declare file descriptor set
    fd_set fds;
    // Max time to wait for a select call
    struct timeval timeout;
    int result;
    // Initialise to an empty set
    FD_ZERO(&fds);
    FD_SET(clientSocket, &fds);
    // Seconds
    timeout.tv_sec = timeoutSeconds;
    // Microseconds
    timeout.tv_usec = 0;
    // Select sees if the specified file descriptor has data ready to read 
    result = select(clientSocket + 1, &fds, NULL, NULL, &timeout);

    if (result < 0) {
        perror("Error in select");
        return 0; // Error
    } else if (result == 0) {
        printf("Timeout: No response received.\n");
        return 0; // Timeout
    } else {
        // ACK received, proceed with reading the packet
        socklen_t serverLength = sizeof(serverAddr);
        int n = recvfrom(clientSocket, packet, sizeof(*packet), 0, (struct sockaddr *)&serverAddr, &serverLength);

        if (n < 0) {
            perror("Error receiving data");
            return 0; // Error
        }

        return 1; // Response received
    }
}

// Driver code
int main() {
    int clientSocket;
    char buffer[MAXLINE];
    struct sockaddr_in serveraddr;
    socklen_t serverLength;
    struct PaymentPacket packet;
    struct timeval timeout;

    // Socket file descriptor - int socket(int domain, int type, int protocol)
    // AF_INET - Internet Protocol v4 address domain
    // SOCK_DGRAM - a Datagram UDP Socket
    // 0 - daddress family's default protocol
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));

    // Filling server information
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    int n, len;

    struct PermissionPacket permission_packet[4];
    for (int i = 0; i < 4; i++) {
        // Populate the constant client ID
        permission_packet[i].start_of_packet = 0XFFFF;
        permission_packet[i].client_id = i;
        permission_packet[i].acc_per = 1;
        permission_packet[i].segment_no = 1;
        permission_packet[i].length = sizeof(struct PermissionPacket);
        permission_packet[i].end_of_packet = 0XFFFF;

        if (i == 0) {
            strcpy(permission_packet[i].technology, "04");
            permission_packet[i].source_subscriber_no = 4085546805;
        }
        // Subscriber does not exist
        else if (i == 1) {
            strcpy(permission_packet[i].technology, "02");
            permission_packet[i].source_subscriber_no = 4099345999;
        }
        // technology does not exist
        else if (i == 2) {
            strcpy(permission_packet[i].technology, "06");
            permission_packet[i].source_subscriber_no = 4085546805;
        }
        // Subscriber has not paid
        else {
            strcpy(permission_packet[i].technology, "03");
            permission_packet[i].source_subscriber_no = 4086668821;
        }
    }

    for (int i = 0; i < 4; i++) {
        // Send data packet to the server
        sendPacket(clientSocket, serveraddr, permission_packet[i]);
        printf("\nPermissions Requested\n");
        int retryCount = 0;
        while (retryCount < MAX_RETRIES) {
            // Attempt to receive response with a 3-second timeout
            if (!receivePacketWithTimeout(clientSocket, serveraddr, &packet, 3)) {
                printf("No response received for Permission Request %d within the timeout. Retrying...\n", i + 1);
                // Retransmit the data packet to the server
                sendPacket(clientSocket, serveraddr, permission_packet[i]);
                retryCount++;
            } else {
                printf("Following response Received from Server: \n");
                // Check if the received packet is a Denied Permission
                if (packet.payment != 0XFFFB) {
                    printf("________________________________________\n");
                    printf("|%u|%u|%u|%u|%u|%s|%u|%u|\n", packet.start_of_packet, packet.client_id, packet.payment,
                           packet.segment_no, packet.length, packet.technology, packet.source_subscriber_no,
                           packet.end_of_packet);
                    printf("________________________________________\n");
                    if (packet.payment == 0XFFFA) {
                        printf("Subscriber does not exist or Technology does not match.\n");
                    } else {
                        printf("Subscriber has not Paid.\n");
                    }

                } else {
                    printf("Transmission successful. Permission was Granted \n");
                    printf("________________________________________\n");
                    printf("|%u|%u|%u|%u|%u|%s|%u|%u|\n", packet.start_of_packet, packet.client_id, packet.payment,
                           packet.segment_no, packet.length, packet.technology, packet.source_subscriber_no,
                           packet.end_of_packet);
                    printf("________________________________________\n");
                }
                break;
            }
        }
        if (retryCount == MAX_RETRIES) {
            printf("No response received for Data Packet %d after %d retries. Transmission unsuccessful.\n", i,
                   MAX_RETRIES);
            break;
        }
    }
    close(clientSocket);

    return 0;
}
// 07700006172 Aishwarya Gupta Assignment 2 - client.c
