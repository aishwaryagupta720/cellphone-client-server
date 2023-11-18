// 07700006172 Aishwarya Gupta Assignment 1 - custom_packet.h

#ifndef CUSTOM_PACKET_H
#define CUSTOM_PACKET_H

#include <stdint.h>

// DataPacket structure
    struct DataPacket {
        int length;  // Length of the entire packet (header + payload)
        uint8_t client_id;
        uint8_t segment_no;
        char type[3];    // {0x44, 0x41, 0x54, 0x41}; DT
        uint16_t start_of_packet;
        uint16_t end_of_packet;
        
        // Other header fields...
        char payload[255];  // Payload data
    };
    // // ACK Packet structure
    // struct AckPacket {
    //     uint8_t client_id;
    //     uint8_t segment_no;
    //     char type[2];    //  = {0x41, 0x43, 0x4B} AK
    //     uint16_t start_of_packet;
    //     uint16_t end_of_packet;
    // };
    // Server Packet structure
    struct ServerPacket {
        uint16_t reject_code;  // Length of the entire packet (header + payload)
        uint8_t client_id;
        uint8_t segment_no;
        char type[3];    // {0x52, 0x45, 0x4A, 0x45, 0x43, 0x54} RJ
        uint16_t start_of_packet;
        uint16_t end_of_packet;
    };

#endif  // CUSTOM_PACKET_H
// 07700006172 Aishwarya Gupta Assignment 1 - custom_packet.h
