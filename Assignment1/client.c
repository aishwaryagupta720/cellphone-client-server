// 07700006172 Aishwarya Gupta Assignment 1 - client.c
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

#define PORT	 8080
#define MAXLINE sizeof(struct DataPacket)
#define MSG_CONFIRM 0
#define MAX_RETRIES 3

// Function to handle errors and exit
void die(char *s) {
    perror(s);
    exit(1);
}

// Function to send a UDP packet to the server
void sendPacket(int clientSocket, struct sockaddr_in serverAddr, struct DataPacket packet) {
    socklen_t serverLength = sizeof(serverAddr);
    printf("Sending Data Packet to Server \n");
    int n = sendto(clientSocket, &packet, sizeof(struct DataPacket), 0, (struct sockaddr *)&serverAddr, serverLength);
    if (n < 0) {
        die("Error sending data");
    }
}

// Function to receive a UDP packet with a timeout
int receivePacketWithTimeout(int clientSocket, struct sockaddr_in serverAddr, struct ServerPacket *packet, int timeoutSeconds) {
    // declare file descriptor set
    fd_set fds;
    // max time to wait for a select call
    struct timeval timeout;
    int result;
    // initialise to empty set
    FD_ZERO(&fds);
    FD_SET(clientSocket, &fds);
    // seconds
    timeout.tv_sec = timeoutSeconds;
    // microseconds
    timeout.tv_usec = 0;
    // select sees if the specified file descriptor has data ready to read 
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
    struct ServerPacket packet;
    struct timeval timeout;

    // Socket file descriptor - int socket(int domain, int type, int protocol)
    // AF_INET - Internet Protocol v4 address domain
    // SOCK_DGRAM - a Datagram UDP Socket
    // 0 - daddress family's default protocol
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Socket Created\n");
    }

    memset(&serveraddr, 0, sizeof(serveraddr));

    // Filling server information
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    int n, len;

    struct DataPacket datapacket[5];

    // Populate the constant client ID
    printf("Please enter your client ID : ");
    int client_id;
    scanf("%d", &client_id);

    // Populate the data packets
    for (int i = 0; i < 5; i++) {
        getchar();
        datapacket[i].client_id = client_id;
        printf("Populate Data Packet %d:\n", i + 1);
        printf("Add start and end of packet (y/n) : ");
        char startend[2];
        scanf("%1s", startend);
        getchar();
        if (strcmp(startend, "y") == 0) {
            datapacket[i].start_of_packet = 0XFFFF;
            datapacket[i].end_of_packet = 0XFFFF;
        } else {
            datapacket[i].start_of_packet = 0;
            datapacket[i].end_of_packet = 0;
        }
        printf("Enter Segment Number : ");
        scanf("%hhu", &datapacket[i].segment_no);
        getchar();
        printf("Default Data Packet length (y/n): ");
        char length[2];
        scanf("%1s", length);
        getchar();
        if (strcmp(length, "n") == 0) {
            printf("Enter Packet Length: ");
            scanf("%d", &datapacket[i].length);
            getchar();
        } else {
            datapacket[i].length = sizeof(datapacket[i]);
        }

        printf("Enter Payload for datapacket : ");
        fgets(datapacket[i].payload, sizeof(datapacket[i].payload), stdin);
        datapacket[i].payload[strcspn(datapacket[i].payload, "\n")] = '\0';
    }

    for (int i = 0; i < 5; i++) {

        // Send data packet to server
        sendPacket(clientSocket, serveraddr, datapacket[i]);

        int retryCount = 0;
        while (retryCount < MAX_RETRIES) {

            // Attempt to receive response with a 3-second timeout
            if (!receivePacketWithTimeout(clientSocket, serveraddr, &packet, 3)) {
                printf("No response received for Data Packet %d within the timeout. Retrying...\n", i+1);
                // Retransmit the data packet to the server
                sendPacket(clientSocket, serveraddr, datapacket[i]);
                retryCount++;
            } else {
                // Check if the received packet is a Reject Packet
                if (strcmp(packet.type, "RJ") == 0) {
                    printf("Received REJECT for Packet %d for Client %u -\n", packet.segment_no, packet.client_id);
                    printf("__________________________\n");
                    printf("|%u|%u|%s|%u|%u|%u|\n", packet.start_of_packet, packet.client_id, packet.type, packet.reject_code, packet.segment_no, packet.end_of_packet);
                    printf("__________________________\n");
                    if (packet.reject_code == 0XFFF4) {
                        printf("Packet out of sequence error.\n");
                    } else if (packet.reject_code == 0XFFF5) {
                        printf("Length Mismatch.\n");
                    } else if (packet.reject_code == 0XFFF6) {
                        printf("End of Packet missing.\n");
                    } else {
                        printf("Duplicate Packet.\n");
                    }

                } else {
                    printf("Transmission successful. Received ACK for Data Packet %d\n", datapacket[i].segment_no);
                    printf("___________________\n");
                    printf("|%u|%u|%s|%u|%u|\n", packet.start_of_packet, packet.client_id, packet.type, packet.segment_no, packet.end_of_packet);
                    printf("___________________\n");
                }
                break;
            }
        }
        if (retryCount == MAX_RETRIES) {
            printf("No response received for Data Packet %d after %d retries. Server did not Respond.\n", i, MAX_RETRIES);
            break;
        }
    }
    close(clientSocket);

    return 0;
}
// 07700006172 Aishwarya Gupta Assignment 1 - client.c