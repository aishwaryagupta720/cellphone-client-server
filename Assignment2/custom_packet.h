// 07700006172 Aishwarya Gupta Assignment 2 - custom_packet.h
#ifndef CUSTOM_PACKET_H
#define CUSTOM_PACKET_H

#include <stdint.h>

// Access Permission Packet structure
    struct PermissionPacket {
        uint8_t length;  // Length of the entire packet (header + payload)
        uint8_t client_id;
        uint8_t segment_no;
        uint16_t start_of_packet;
        uint16_t end_of_packet;
        uint16_t acc_per;
        char technology[3];
        uint32_t source_subscriber_no;  // Payload data
    };

// Payment Packet structure
    struct PaymentPacket {
        uint8_t length;  // Length of the entire packet (header + payload)
        uint8_t client_id;
        uint8_t segment_no;
        uint16_t start_of_packet;
        uint16_t end_of_packet;
        uint16_t payment;
        char technology[3];
        uint32_t source_subscriber_no;  // Payload data
    };


// File Records
    struct Record {
        uint32_t SubscriberNumber;
        char Technology[2];
        int Paid;
    };

#endif  // CUSTOM_PACKET_H
// 07700006172 Aishwarya Gupta Assignment 2 - custom_packet.h
