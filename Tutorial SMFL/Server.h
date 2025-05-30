#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <memory>
#include "ClientManager.h"
#include "EventManager.h"
#include "DatabaseManager.h"
#include "MatchMakingManager.h"

#define LISTENER_PORT 55001
//const sf::IpAddress DEDICATED_SERVER_IP = sf::IpAddress(192, 168, 1, 144);
const sf::IpAddress DEDICATED_SERVER_IP = sf::IpAddress(10, 40, 1, 99);
const int DEDICATED_SERVER_PORT = 55001;

class Server {
private:
    sf::TcpListener listener;
    sf::SocketSelector socketSelector;

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

