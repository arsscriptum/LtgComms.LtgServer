#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

// Constants
constexpr int PORT = 12345; // Server port
constexpr char SERVER_IP[] = "127.0.0.1"; // Server IP address
constexpr int BUFFER_SIZE = 256; // Buffer size for sending data

// Packet structure
struct Packet {
    uint8_t startByte;
    uint8_t eventId;
    uint8_t length;
    uint8_t data[31];
    uint8_t checksum;
};

// Function to calculate checksum (simple XOR of all bytes)
uint8_t calculateChecksum(const Packet &packet) {
    uint8_t checksum = 0;
    checksum ^= packet.startByte;
    checksum ^= packet.eventId;
    checksum ^= packet.length;
    for (int i = 0; i < packet.length; ++i) {
        checksum ^= packet.data[i];
    }
    return checksum;
}

// Function to create a packet
Packet createPacket(uint8_t startByte, uint8_t eventId, const std::vector<uint8_t>& data) {
    Packet packet;
    packet.startByte = startByte;
    packet.eventId = eventId;
    packet.length = data.size();
    std::memcpy(packet.data, data.data(), data.size());
    packet.checksum = calculateChecksum(packet);
    return packet;
}

// Function to serialize packet to buffer
std::vector<uint8_t> serializePacket(const Packet &packet) {
    std::vector<uint8_t> buffer;
    buffer.push_back(packet.startByte);
    buffer.push_back(packet.eventId);
    buffer.push_back(packet.length);
    for (int i = 0; i < packet.length; ++i) {
        buffer.push_back(packet.data[i]);
    }
    buffer.push_back(packet.checksum);
    return buffer;
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Fill server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Create a packet to send
    std::vector<uint8_t> data = {0x01, 0x02, 0x03}; // Example data
    Packet packet = createPacket(0xAA, 0x01, data);

    // Serialize the packet
    std::vector<uint8_t> buffer = serializePacket(packet);

    // Send the packet
    if (sendto(sockfd, buffer.data(), buffer.size(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Packet sent to server\n";

    close(sockfd);
    return 0;
}
