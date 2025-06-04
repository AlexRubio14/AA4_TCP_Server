#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <memory>
#include "ClientManager.h"
#include "EventManager.h"
#include "DatabaseManager.h"
#include "MatchMakingManager.h"

#define LISTENER_PORT 55001
const sf::IpAddress DEDICATED_SERVER_IP_LOCAL = sf::IpAddress(192, 168, 1, 144);
//const sf::IpAddress DEDICATED_SERVER_IP = sf::IpAddress(10, 40, 1, 99);
const sf::IpAddress DEDICATED_SERVER_IP_PUBLIC = sf::IpAddress(93, 176, 163, 135);

const int DEDICATED_SERVER_PORT = 55002;

class Server {
private:
    sf::TcpListener listener;
    sf::SocketSelector socketSelector;
	std::shared_ptr<sf::UdpSocket> udpSocket;

    bool isRunning;

    ClientManager& clientManager;
    EventManager& eventManager;
    DatabaseManager& databaseManager;
    PacketManager& packetManager;
    MatchMakingManager matchmakingManager;

public:

    Server();

    ~Server();

    void Start();

    void Update();

    void HandleNewConnection();
    void HandleDisconnection(const std::string& guid);
};

