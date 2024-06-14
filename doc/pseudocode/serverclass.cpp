#include <iostream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

constexpr int PORT = 12345;
constexpr int BUFFER_SIZE = 256;

class LtgUdpServer {
public:
    LtgUdpServer() : sockfd(INVALID_SOCKET) {
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }

    ~LtgUdpServer() {
        if (sockfd != INVALID_SOCKET) {
            closesocket(sockfd);
        }
        WSACleanup();
    }

    bool init() {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd == INVALID_SOCKET) {
            perror("Socket creation failed");
            return false;
        }

        sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(PORT);

        if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
            perror("Bind failed");
            closesocket(sockfd);
            return false;
        }

        std::cout << "Server listening on port " << PORT << "...\n";
        return true;
    }

    void run() {
        while (true) {
            uint8_t buffer[BUFFER_SIZE];
            sockaddr_in cliaddr;
            int len = sizeof(cliaddr);
            int n = recvfrom(sockfd, reinterpret_cast<char*>(buffer), BUFFER_SIZE, 0, (struct sockaddr*)&cliaddr, &len);
            if (n == SOCKET_ERROR) {
                perror("Receive failed");
                continue;
            }

            // Add received data to the FIFO queue
            dataBuffer.insert(dataBuffer.end(), buffer, buffer + n);

            while (true) {
                Packet packet;
                if (parsePacket(packet)) {
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
                    removeParsedData(packet.length + 4);
                } else {
                    break; // Not enough data to parse a packet
                }
            }
        }
    }

private:
    struct Packet {
        uint8_t startByte;
        uint8_t eventId;
        uint8_t length;
        uint8_t data[31];
        uint8_t checksum;
    };

    SOCKET sockfd;
    WSADATA wsaData;
    std::vector<uint8_t> dataBuffer;

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

    bool parsePacket(Packet &packet) {
        if (dataBuffer.size() < 4) {
            return false; // Not enough data to parse
        }
        packet.startByte = dataBuffer[0];
        packet.eventId = dataBuffer[1];
        packet.length = dataBuffer[2];
        if (packet.length > 31 || dataBuffer.size() < packet.length + 4) {
            return false; // Not enough data or invalid length
        }
        std::memcpy(packet.data, &dataBuffer[3], packet.length);
        packet.checksum = dataBuffer[3 + packet.length];

        if (calculateChecksum(packet) != packet.checksum) {
            return false; // Checksum mismatch
        }
        return true;
    }

    void removeParsedData(int length) {
        dataBuffer.erase(dataBuffer.begin(), dataBuffer.begin() + length);
    }
};

int main() {
    LtgUdpServer server;
    if (server.init()) {
        server.run();
    } else {
        std::cerr << "Failed to initialize server\n";
    }
    return 0;
}

