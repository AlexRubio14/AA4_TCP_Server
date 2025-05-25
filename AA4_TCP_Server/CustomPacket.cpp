#include "CustomPacket.h"



CustomPacket::CustomPacket(PacketType type)
{
	this->type = type;
	packet << type;
}

sf::Packet& operator>>(sf::Packet& packet, PacketType& type)
{
	int temp;
	packet >> temp;
	type = static_cast<PacketType>(temp);

	return packet;
}

sf::Packet& operator<<(sf::Packet& packet, const PacketType& type)
{
	packet << static_cast<int>(type);
	return packet;
}
