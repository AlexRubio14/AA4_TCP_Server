#pragma once
#include "Client.h"
#include <memory>
#include <deque>
class MatchMakingManager
{
private:
	std::deque<std::shared_ptr<Client>> matchmakingQueue;
	sf::IpAddress dedicatedServerIpLocal;
	sf::IpAddress dedicatedServerIpPublic;
	int dedicatedServerPort;
	std::shared_ptr<sf::UdpSocket> udpSocket;

public:
	MatchMakingManager();
	MatchMakingManager(sf::IpAddress DEDICATED_SERVER_IP_LOCAL, sf::IpAddress DEDICATED_SERVER_IP_PUBLIC, int DEDICATED_SERVER_PORT, std::shared_ptr<sf::UdpSocket> udpSocket);

	void EnqueuePlayer(const std::shared_ptr<Client>& client);
	bool DequeuePlayer(const std::shared_ptr<Client>& client);
	void ProcessMatches();

	void SendMatchFoundToClient(const std::shared_ptr<Client>& player, int enemyId); // Data send to clients to know their gameID and connect to udpServer
	void SendMatchFoundToUdpServer(const std::shared_ptr<Client>& player1, const std::shared_ptr<Client>& player2); // Data send to udp server with ip and port of players
};

