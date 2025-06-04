#pragma once
#include <SFML/Network.hpp>
#include "PacketType.h"
#include "UDPPacketType.h"
#include <iostream>

class CustomUDPPacket
{
public:
    CustomUDPPacket() = default;
    CustomUDPPacket(UdpPacketType udpType, PacketType type, int PlayerId);

    void ReadBuffer(const char* inputBuffer, size_t _bufferSize);

    PacketType type;
    UdpPacketType udpType;
    int playerId;

    char buffer[1024];
    size_t bufferSize;

    size_t payloadOffset;

    template<typename T>
    bool WriteVariable(const T& data)
    {
        if (bufferSize + sizeof(T) > sizeof(buffer)) {
            // Buffer overflow, no escribimos
            return false;
        }
        std::memcpy(buffer + bufferSize, &data, sizeof(T));
        bufferSize += sizeof(T);
        return true;
    }

    template<typename T>
    bool ReadVariable(T& outData, size_t& offset) const {
        if (offset + sizeof(T) > bufferSize)
            return false; // No hay suficiente espacio para leer

        std::memcpy(&outData, buffer + offset, sizeof(T));
        offset += sizeof(T);
        return true;
    }

    bool WriteString(const std::string& str);
};