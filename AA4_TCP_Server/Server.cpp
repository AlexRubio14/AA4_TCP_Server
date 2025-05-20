#include "Server.h"
#include "Client.h"
#include "EventManager.h"
#include "DatabaseManager.h"
#include "RoomManager.h"

Server::Server()
{
    isRunning = true;
}

void Server::Start()
{
    if (listener.listen(LISTENER_PORT) == sf::Socket::Status::Done)
    {
        std::cout << "Server launched at port: " << LISTENER_PORT << std::endl;

        socketSelector.add(listener);

        DB_MANAGER.ConnectDb();

        EVENT_MANAGER.Subscribe(DISCONNECT, [this](std::string guid, CustomPacket& customPacket) {
            
			std::function<void(std::shared_ptr<Client>)> removeSocket = [this](std::shared_ptr<Client> client)
            {
                socketSelector.remove(client->GetSocket());
                std::cout << "Socket from: " << client->GetSocket().getRemoteAddress().value() << " erased in socketSelector" << std::endl;
			};

            std::shared_ptr<Client> client;

            if (client = CLIENT_MANAGER.GetAuthoritedClientById(guid))
				removeSocket(client);
            else if (client = CLIENT_MANAGER.GetPendingClientById(guid))
				removeSocket(client);
            else 
                std::cerr << "Trying to disconnect non-existing client ( guid = " << guid << ")" << std::endl;
            
            CLIENT_MANAGER.DisconnectClient(guid);

            std::cout << std::endl << std::endl << client->GetIsInRoom() << std::endl << std::endl;

            if (client->GetIsInRoom())
			    ROOM_MANAGER.LeaveRoom(client->GetCurrentRoomId(), client);

        });

		PACKET_MANAGER.Init();
    }
    else
    {
        isRunning = false;
        std::cerr << "I can't listen to the port: " << LISTENER_PORT << std::endl;
        return;
    }
}

void Server::Update()
{
    while (isRunning) 
    {
        if (socketSelector.wait()) 
        {
            if (socketSelector.isReady(listener)) 
            {
                HandleNewConnection();
            }
            else 
            {
                CLIENT_MANAGER.UpdateClients(socketSelector);
            }
        }
    }

    DB_MANAGER.DisconnectDb();
}

void Server::HandleNewConnection()
{
   Client& newClient = CLIENT_MANAGER.CreateClient();

    if (listener.accept(newClient.GetSocket()) == sf::Socket::Status::Done)
    {
        newClient.GetSocket().setBlocking(false);
        socketSelector.add(newClient.GetSocket());
    }
}

Server::~Server()
{
}
