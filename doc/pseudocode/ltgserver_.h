#ifndef LTGUDPSERVER_H
#define LTGUDPSERVER_H

#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

class LtgUdpServer {
public:
    LtgUdpServer();
    ~LtgUdpServer();
    bool init();
    void run();

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

    uint8_t calculateChecksum(const Packet &packet);
    bool parsePacket(Packet &packet);
    void removeParsedData(int length);
};

#endif // LTGUDPSERVER_H
