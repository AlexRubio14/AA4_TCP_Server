#include "MatchMakingManager.h"
#include <iostream>
#include "PacketManager.h"
#include "EventManager.h"

MatchMakingManager::MatchMakingManager(sf::IpAddress DEDICATED_SERVER_IP, int DEDICATED_SERVER_PORT)
	: dedicatedServerIP(DEDICATED_SERVER_IP), dedicatedServerPort(DEDICATED_SERVER_PORT) {}

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
	}
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

		std::cout << "Match formed between " << player1->GetUsername() << " and " << player2->GetUsername() << std::endl;

		//TODO: send to dedicated server the info of this 2 players

		CustomPacket responsePacket(PacketType::START_GAME);
		responsePacket.packet << dedicatedServerIP.toString() << dedicatedServerPort;
		EVENT_MANAGER.Emit(PacketType::START_GAME, player1->GetGuid(), responsePacket);
		EVENT_MANAGER.Emit(PacketType::START_GAME, player2->GetGuid(), responsePacket);
	}
}
