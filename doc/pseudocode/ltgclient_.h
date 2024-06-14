#ifndef LTGUDPCLIENT_H
#define LTGUDPCLIENT_H

#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

class LtgUdpClient {
public:
    LtgUdpClient();
    ~LtgUdpClient();
    bool init();
    void sendPacket(uint8_t startByte, uint8_t eventId, const std::vector<uint8_t>& data);

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

    uint8_t calculateChecksum(const Packet &packet);
    Packet createPacket(uint8_t startByte, uint8_t eventId, const std::vector<uint8_t>& data);
    std::vector<uint8_t> serializePacket(const Packet &packet);
};

#endif // LTGUDPCLIENT_H
