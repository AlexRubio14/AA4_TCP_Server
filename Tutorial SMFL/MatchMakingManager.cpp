#include "MatchMakingManager.h"
#include <iostream>
#include "PacketManager.h"
#include "EventManager.h"
#include "ClientManager.h"
#include "CustomUDPPacket.h"

MatchMakingManager::MatchMakingManager(sf::IpAddress DEDICATED_SERVER_IP_LOCAL, sf::IpAddress DEDICATED_SERVER_IP_PUBLIC, int DEDICATED_SERVER_PORT, std::shared_ptr<sf::UdpSocket> udpSocket)
	: dedicatedServerIpLocal(DEDICATED_SERVER_IP_LOCAL), dedicatedServerIpPublic(DEDICATED_SERVER_IP_PUBLIC), dedicatedServerPort(DEDICATED_SERVER_PORT), udpSocket(udpSocket) { }

void MatchMakingManager::EnqueuePlayer(const std::shared_ptr<Client>& client)
{
	matchmakingQueue.push_back(client);
	std::cout << "Client " << client->GetUsername() << " has started a queue." << std::endl;
	ProcessMatches();
}

bool MatchMakingManager::DequeuePlayer(const std::shared_ptr<Client>& client)
{
	auto it = std::find(matchmakingQueue.begin(), matchmakingQueue.end(), client);
	if (it != matchmakingQueue.end())
	{
		matchmakingQueue.erase(it);
		std::cout << "Client " << client->GetUsername() << " has been removed from the matchmaking queue." << std::endl;
		return true;
	}

	return false;
}

void MatchMakingManager::ProcessMatches()
{
	if (matchmakingQueue.size() < 2)
	{
		std::cout << "Not enough players to form a match." << std::endl;
		return;
	}

	while (matchmakingQueue.size() >= 2)
	{
		// Pop two players from the queue to form a match
		std::shared_ptr<Client> player1 = matchmakingQueue.front();
		matchmakingQueue.pop_front();
		std::shared_ptr<Client> player2 = matchmakingQueue.front();
		matchmakingQueue.pop_front();

		player1->SetGameId(CLIENT_MANAGER.TakeNextClientId());
		player2->SetGameId(CLIENT_MANAGER.TakeNextClientId());

		std::cout << "Match formed between " << player1->GetUsername() << " and " << player2->GetUsername() << std::endl;

		SendMatchFoundToClient(player1, player2->GetGameId());
		SendMatchFoundToClient(player2, player1->GetGameId());

		SendMatchFoundToUdpServer(player1, player2);
	}
}

// Data send to clients to know their gameID and connect to udpServer
void MatchMakingManager::SendMatchFoundToClient(const std::shared_ptr<Client>& player, int enemyId)
{
	CustomPacket responsePacketPlayer1(PacketType::MATCH_FOUND);
	responsePacketPlayer1.packet << player->GetGameId() << enemyId << dedicatedServerIpPublic.toString() << dedicatedServerPort;
	PACKET_MANAGER.SendPacketToClient(player, responsePacketPlayer1);
}

// Data send to udp server with ip and port of players
void MatchMakingManager::SendMatchFoundToUdpServer(const std::shared_ptr<Client>& player1, const std::shared_ptr<Client>& player2)
{
	CustomUDPPacket matchPacket(UdpPacketType::NORMAL, MATCH_FOUND, 0);

	// Write player 1 data
	std::string player1Ip = player1->GetSocket().getRemoteAddress()->toString();

	if (!matchPacket.WriteVariable(player1->GetGameId()))
	{
		std::cout << "No se ha podido escribir: " << player1->GetGameId() << std::endl;
	}

	//Write player 2 data
	std::string player2Ip = player2->GetSocket().getRemoteAddress()->toString();

	if (!matchPacket.WriteVariable(player2->GetGameId()))
	{
		std::cout << "No se ha podido escribir player2id: " << player2->GetGameId() << std::endl;
	}

	std::cout << " el paquete es del tipo" << static_cast<int>(matchPacket.type) << std::endl;

	if (udpSocket->send(matchPacket.buffer, matchPacket.bufferSize, dedicatedServerIpLocal, dedicatedServerPort) == sf::Socket::Status::Done)
		std::cout << "Match data sent to UDP server for players: " << player1->GetUsername() << " and " << player2->GetUsername() << std::endl;
	else
		std::cerr << "Failed to send match data to UDP server." << std::endl;
}
