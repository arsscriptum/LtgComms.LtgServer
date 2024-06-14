#include <iostream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

constexpr int PORT = 12345;
constexpr char SERVER_IP[] = "127.0.0.1";
constexpr int BUFFER_SIZE = 256;

class LtgUdpClient {
public:
    LtgUdpClient() : sockfd(INVALID_SOCKET) {
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }

    ~LtgUdpClient() {
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

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
            perror("Invalid address/ Address not supported");
            closesocket(sockfd);
            return false;
        }

        return true;
    }

    void sendPacket(uint8_t startByte, uint8_t eventId, const std::vector<uint8_t>& data) {
        Packet packet = createPacket(startByte, eventId, data);
        std::vector<uint8_t> buffer = serializePacket(packet);

        if (sendto(sockfd, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0, 
                   (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
            perror("Send failed");
            return;
        }

        std::cout << "Packet sent to server\n";
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
    sockaddr_in servaddr;

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

    Packet createPacket(uint8_t startByte, uint8_t eventId, const std::vector<uint8_t>& data) {
        Packet packet;
        packet.startByte = startByte;
        packet.eventId = eventId;
        packet.length = data.size();
        std::memcpy(packet.data, data.data(), data.size());
        packet.checksum = calculateChecksum(packet);
        return packet;
    }

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
};

int main() {
    LtgUdpClient client;
    if (client.init()) {
        std::vector<uint8_t> data = {0x01, 0x02, 0x03}; // Example data
        client.sendPacket(0xAA, 0x01, data);
    } else {
        std::cerr << "Failed to initialize client\n";
    }
    return 0;
}
