
//==============================================================================
//
//  udp_server.h
//
//==============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================


#ifndef __LTG_UDPSERVER_H
#define __LTG_UDPSERVER_H

#include "stdafx.h"
#include "udp_server.h"
#include "cthread.h"


#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

class LtgUdpServer : public CThread {
public:
    LtgUdpServer();
    ~LtgUdpServer();
    bool init();
    

protected:
    unsigned long Process(void* parameter);

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

    uint8_t calculateChecksum(const Packet& packet);
    bool parsePacket(Packet& packet);
    void removeParsedData(int length);
};

#endif // __LTG_UDPSERVER_H






