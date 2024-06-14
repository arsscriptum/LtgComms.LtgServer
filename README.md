# Ltg Comms - Ltg Server

This is my custom UDP server for testing

## Details: LtgUdpServer Class Implementation

I have  a class implementing the UDP network communication with basic features

### Initialization and Cleanup

The LtgUdpClient class handles the initialization and cleanup of the Winsock library.
It creates a UDP socket and sets up the server address information.


### Packet Creation and Sending

The sendPacket method creates a packet with the specified start byte, event ID, and data, calculates the checksum, serializes it, and sends it to the server using sendto.
Packet Data Structure:

The Packet structure represents the format of the packets being sent.


### Checksum Calculation:

The calculateChecksum method calculates the checksum for a packet as a simple XOR of all bytes in the packet.
Packet Serialization:

The serializePacket method converts the packet into a std::vector<uint8_t> buffer for sending over the network.