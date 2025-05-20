#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <memory>
//#include "ClientManager.h"

#define LISTENER_PORT 55001

class Server {
private:
    sf::TcpListener listener;
    sf::SocketSelector socketSelector;

    bool isRunning;

public:

    Server();

    ~Server();

    void Start();

    void Update();

    void HandleNewConnection();
};

