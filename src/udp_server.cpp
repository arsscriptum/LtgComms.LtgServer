
//==============================================================================
//
//  udp_server.cpp
//
//==============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================

#include "stdafx.h"
#include "udp_server.h"
#include "log.h"


#include <iostream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>

// Constants
constexpr int PORT = 12345;
constexpr int BUFFER_SIZE = 256;

// Constructor
LtgUdpServer::LtgUdpServer() : sockfd(INVALID_SOCKET) 
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

// Destructor
LtgUdpServer::~LtgUdpServer() 
{
    if (sockfd != INVALID_SOCKET) {
        closesocket(sockfd);
    }
    WSACleanup();
}


unsigned long LtgUdpServer::Process(void* parameter)
{
    printf("thread started\n");
	while (true) {

		Sleep(1000);

		uint8_t buffer[BUFFER_SIZE];
		sockaddr_in cliaddr;
		int len = sizeof(cliaddr);
		int n = recvfrom(sockfd, reinterpret_cast<char*>(buffer), BUFFER_SIZE, 0, (struct sockaddr*)&cliaddr, &len);
		if (n == SOCKET_ERROR) {
			perror("Receive failed");
			continue;
		}
        printf("[*] received data (%d bytes). ", n);
		// Add received data to the FIFO queue
		dataBuffer.insert(dataBuffer.end(), buffer, buffer + n);

		while (true) {
			Packet packet;
            printf("attempt to parse...");
			if (parsePacket(packet)) {
                printf(" detected valid packet!\n");
                printf("  [*] magic byte    :  %d\n", static_cast<int>(packet.startByte));
                printf("  [*] function code :  %d\n", static_cast<int>(packet.eventId));
                printf("  [*] data length   :  %d\n", static_cast<int>(packet.length));
                printf("  [*] data values   :  ");

				for (int i = 0; i < packet.length; ++i) {
                    printf("%02x ", static_cast<int>(packet.data[i]));
					
				}
                printf("\n");
                printf("  [*] checksum     :  %d\n", static_cast<int>(packet.checksum));

				// Remove parsed packet data from the buffer
				removeParsedData(packet.length + 4);
			}
			else {
                printf("incomplete.\n");
				break; // Not enough data to parse a packet
			}
		}
	}
}

// Initialize server
bool LtgUdpServer::init() {
    COUTC("[LtgUdpServer] Initialize server\n");

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


// Calculate checksum
uint8_t LtgUdpServer::calculateChecksum(const Packet& packet) {
    uint8_t checksum = 0;
    checksum ^= packet.startByte;
    checksum ^= packet.eventId;
    checksum ^= packet.length;
    for (int i = 0; i < packet.length; ++i) {
        checksum ^= packet.data[i];
    }
    return checksum;
}

// Parse packet
bool LtgUdpServer::parsePacket(Packet& packet) {

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

// Remove parsed data
void LtgUdpServer::removeParsedData(int length) {
    dataBuffer.erase(dataBuffer.begin(), dataBuffer.begin() + length);
}
