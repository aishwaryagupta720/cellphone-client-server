// 07700006172 Aishwarya Gupta Assignment 2 - server.c
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
	
#define PORT	 8888
#define MAXLINE sizeof(struct PermissionPacket)
#define MSG_CONFIRM 0
#define PACKET_MARKER 0XFFFF

// Function to handle errors and exit
void die(char *s) {
    perror(s);
    exit(1);
}

// Function to search for records in the file
int searchRecords(const char *filename, uint32_t targetSubscriberNumber, const char *targetTechnology) {
    // Open the file for reading
    FILE *file = fopen(filename, "r");

    char firstLine[100]; 
    if (fgets(firstLine, sizeof(firstLine), file) == NULL) {
        perror("Error opening file");
        fclose(file);
        return -2; // File is empty or reading error
    }
    // Read the file line by line
    char line[100];  // Adjust the size according to your needs
    while (fgets(line, sizeof(line), file) != NULL) {
        // Parse the line into fields
        struct Record record;
        if (sscanf(line, "%u %2s %d", &record.SubscriberNumber, record.Technology, &record.Paid) != 3) {
            fprintf(stderr, "Error parsing line: %s", line);
            continue;
        }

        // Compare with the target variables
        if (record.SubscriberNumber == targetSubscriberNumber) {
            if (strcmp(record.Technology, targetTechnology) == 0) {
                // Subscriber found with matching technology
                fclose(file);
                return record.Paid; // Return Paid status
            } else {
                // Subscriber found but technology doesn't match
                fclose(file);
                return -1;
            }
        }
    }

    // Subscriber not found
    fclose(file);
    return -2;
}

// Driver code
int main() {
    int serverSocket, clientSocket;
    char buffer[MAXLINE];
    socklen_t clientLength;
    // IPV4 socket address structure
    struct sockaddr_in serveraddr, clientaddr;
    struct PaymentPacket packet;
    struct PermissionPacket permissionpacket;

    // Filling in packet, rest will be filled in while loop
    packet.start_of_packet = PACKET_MARKER;
    packet.end_of_packet = PACKET_MARKER;

    // Creating socket file descriptor
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    } else {
        printf("Socket Created Successfully");
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
            sizeof(serveraddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Bind Successful");
    }
    listen(serverSocket, 5);

    int len, n;
    int received_packets = 0;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientaddr, &addr_size);

    while (1) {
        // Receive data packet from client
        clientLength = sizeof(clientaddr);
        n = recvfrom(serverSocket, &permissionpacket, sizeof(permissionpacket), 0, (struct sockaddr *)&clientaddr, &clientLength);
        if (n < 0) {
            die("Error receiving data");
        }
        printf("\nReceived Client Permission Packet \nClient: %u\nTechnology: %s\nSubscriber Number: %u\n", permissionpacket.client_id, permissionpacket.technology, permissionpacket.source_subscriber_no);
        int result = searchRecords("Verification_Database.txt", permissionpacket.source_subscriber_no, permissionpacket.technology);

        // Populate Response Payment Packet
        packet.client_id = permissionpacket.client_id;
        packet.segment_no = permissionpacket.segment_no;
        strcpy(packet.technology, permissionpacket.technology);
        packet.source_subscriber_no = permissionpacket.source_subscriber_no;
        packet.length = sizeof(struct PaymentPacket);
         if (result == -2){
            // For if Subscriber number is Not Found in Database
            packet.payment = 0XFFFA;
            sendto(serverSocket, &packet, sizeof(packet) , 0, (struct sockaddr *)&clientaddr, clientLength);
            printf("Subscriber number is not found. Request Denied\n");
            continue;
        } 
        else if (result == -1) {
            // When Subscriber number found but technology doesn't match
            packet.payment = 0XFFFA;
            sendto(serverSocket, &packet, sizeof(packet) , 0, (struct sockaddr *)&clientaddr, clientLength);
            printf("Subscriber number found but Technology does not match. Request Denied\n");
            continue;
        } 
        else if (result == 0) {
            // When Subscriber has not paid
            packet.payment = 0XFFF9;
            sendto(serverSocket, &packet, sizeof(packet) , 0, (struct sockaddr *)&clientaddr, clientLength);
            printf("Subscriber has not paid. Request Denied\n");
            continue;
        } 
        else {
            // When Subscriber has paid
            packet.payment = 0XFFFB;
            sendto(serverSocket, &packet, sizeof(packet) , 0, (struct sockaddr *)&clientaddr, clientLength);
            printf("Subscriber permitted to access the network.\n");
        } 
    }

    close(serverSocket);

    return 0;
}
// 07700006172 Aishwarya Gupta Assignment 2 - server.c
