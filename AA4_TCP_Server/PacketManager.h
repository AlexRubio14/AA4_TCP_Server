#pragma once
#include "CustomPacket.h"
#include "Client.h"

#define PACKET_MANAGER PacketManager::Instance()


class PacketManager
{
private:

	PacketManager() = default;
	PacketManager(const PacketManager&) = delete;
	PacketManager& operator=(const PacketManager&) = delete;

	void HandleHandshake(sf::Packet& packet);

	void SendHandshake(const std::string guid);

public:

	static PacketManager& Instance();

	void Init();

	void ProcessPacket(std::string guid, CustomPacket customPacket);

	void SendPacketToClient(const std::shared_ptr<Client> client, CustomPacket& responsePacket);
};

