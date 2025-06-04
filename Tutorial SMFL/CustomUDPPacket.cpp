#include "CustomUDPPacket.h"

CustomUDPPacket::CustomUDPPacket(UdpPacketType udpType, PacketType type, int playerId)
{
	bufferSize = 0;
    payloadOffset = 0;

    this->udpType = udpType;
    this->type = type;
    this->playerId = playerId;

	std::cout << "el paquete es de tipo:" << static_cast<int>(type) << std::endl;
    if (!WriteVariable(udpType))
    {
		std::cout << "No se ha podido escribir el udpType: " << static_cast<int>(udpType) << std::endl;
    }
    if (!WriteVariable(type))
    {
        std::cout << "No se ha podido escribir el type: " << static_cast<int>(type) << std::endl;
    }
    if (!WriteVariable(playerId))
    {
        std::cout << "No se ha podido escribir el playerId: " << playerId << std::endl;
    }

    payloadOffset = bufferSize;
}

void CustomUDPPacket::ReadBuffer(const char* inputBuffer, size_t _bufferSize)
{
    size_t offset = 0;

    // Copiamos todo el paquete al buffer interno, incluyendo cabecera y payload
    std::memcpy(buffer, inputBuffer, _bufferSize);
    bufferSize = _bufferSize;

    ReadVariable(udpType, offset);
    ReadVariable(type, offset);
    ReadVariable(playerId, offset);

    payloadOffset = offset;
}

bool CustomUDPPacket::WriteString(const std::string& str)
{
    uint16_t length = static_cast<uint16_t>(str.size());
    if (!WriteVariable(length))
        return false;

    if (bufferSize + length > sizeof(buffer))
        return false;

    std::memcpy(buffer + bufferSize, str.data(), length);
    bufferSize += length;

    return true;
}