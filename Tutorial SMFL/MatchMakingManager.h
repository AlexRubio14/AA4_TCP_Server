#pragma once
#include <deque>
#include "Client.h"
#include <memory>

class MatchMakingManager
{
private:
	std::deque<std::shared_ptr<Client>> matchmakingQueue;
	sf::IpAddress dedicatedServerIP;
	int dedicatedServerPort;

public:

	MatchMakingManager(sf::IpAddress DEDICATED_SERVER_IP, int DEDICATED_SERVER_PORT);

	void EnqueuePlayer(const std::shared_ptr<Client>& client);
	bool DequeuePlayer(const std::shared_ptr<Client>& client);
	void ProcessMatches();
};

