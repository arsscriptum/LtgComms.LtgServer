#include <iostream>
#include <queue>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

// Constants
constexpr int PORT = 12345; // Port to listen on
constexpr int BUFFER_SIZE = 256; // Buffer size for receiving data

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

// Function to parse the buffer into a Packet
bool parsePacket(const std::vector<uint8_t> &buffer, Packet &packet) {
    if (buffer.size() < 4) {
        return false; // Not enough data to parse
    }
    packet.startByte = buffer[0];
    packet.eventId = buffer[1];
    packet.length = buffer[2];
    if (packet.length > 31 || buffer.size() < packet.length + 4) {
        return false; // Not enough data or invalid length
    }
    std::memcpy(packet.data, &buffer[3], packet.length);
    packet.checksum = buffer[3 + packet.length];
    
    if (calculateChecksum(packet) != packet.checksum) {
        return false; // Checksum mismatch
    }
    return true;
}

// Function to remove parsed packet data from the buffer
void removeParsedData(std::vector<uint8_t> &buffer, int length) {
    buffer.erase(buffer.begin(), buffer.begin() + length);
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Fill server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    std::vector<uint8_t> dataBuffer; // FIFO queue for received data

    while (true) {
        uint8_t buffer[BUFFER_SIZE];
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("Receive failed");
            continue;
        }

        // Add received data to the FIFO queue
        dataBuffer.insert(dataBuffer.end(), buffer, buffer + n);

        while (true) {
            Packet packet;
            if (parsePacket(dataBuffer, packet)) {
                std::cout << "Received valid packet:\n";
                std::cout << "Start Byte: " << static_cast<int>(packet.startByte) << "\n";
                std::cout << "Event ID: " << static_cast<int>(packet.eventId) << "\n";
                std::cout << "Length: " << static_cast<int>(packet.length) << "\n";
                std::cout << "Data: ";
                for (int i = 0; i < packet.length; ++i) {
                    std::cout << std::hex << static_cast<int>(packet.data[i]) << " ";
                }
                std::cout << std::dec << "\nChecksum: " << static_cast<int>(packet.checksum) << "\n";

                // Remove parsed packet data from the buffer
                removeParsedData(dataBuffer, packet.length + 4);
            } else {
                break; // Not enough data to parse a packet
            }
        }
    }

    close(sockfd);
    return 0;
}
