# Aishwarya Gupta 07700006172 CodingAssignment1
# Client using customized protocol on top of UDP protocol for sending information to the server.

This is a client-server socket program written in C. It demonstrates communication between a server and a client using UDP sockets.
The client sends five packets (Packet 1, 2, 3, 4, 5) to the server.
The server acknowledges with ACK receive of each correct packet from client by sending five ACKs, one ACK for each 5 received packets.
The client then sends another five packets (Packet 1, 2, 3, 4, 5) to the server, emulating one correct packet and four packets with errors.
The server acknowledges with ACK receive of correct packet from client, and with corresponding Reject sub codes for packets with errors.
The client will start an ack_timer at the time the packet is sent to the server, if the ACK (Acknowledge) for each packet has not been received during ack_timer period by client before expiration of timer then client should retransmit the packet that was sent before.
The timer can be set at 3 seconds (recommended) and a retry counter should be used for resending the packet. If the ACK for the packet does not arrive before the timeout, the client will retransmit the packet and restart the ack_timer, and the ack_timer should be reset for a total of 3 times.
If no ACK was received from the server after resending the same packet 3 times, the client should generate the following message and display on the screen:
“Server does not respond”.

## Instructions:

### Server:

1. Open a terminal.

2. Navigate to the 'server' directory.

3. Compile the server program:
    ```bash
    gcc server.c -o server
    ```

4. Run the server:
    ```bash
    ./server
    ```
   This will start the server, and it will wait for incoming UDP packets.

### Client:

1. Open another terminal.

2. Navigate to the 'CodingAssignment1' directory.

3. Compile the client program:
    ```bash
    clang client.c -o client
    ```

4. Run the client:
    ```bash
    ./client
    ```
   This will prompt you to enter a client ID and populate data packets. The client will then send these packets to the server.

**Note**: Ensure that the server is running before starting the client.

## Customization:

- The programs use a custom packet structure defined in 'custom_packet.h'. You can modify this structure according to your requirements.

- Adjust the IP address and port number in the server and client programs if needed. The default port is set to 8080.

## Troubleshooting:

- If there are any compilation errors, ensure that you have the required libraries installed, and the necessary headers are included.

- Check for proper permissions if there are issues with creating or binding sockets.

- Ensure that there are no conflicts with the specified port number.

- If the client is not receiving ACK or is receiving REJECT packets, check for issues in the data packet structure and validation logic.

