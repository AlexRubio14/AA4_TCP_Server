#include "Server.h"
#include "Client.h"
#include "EventManager.h"
#include "DatabaseManager.h"

Server::Server()
    : clientManager(ClientManager::Instance()),
      packetManager(PacketManager::Instance()),
      eventManager(EventManager::Instance()),
      databaseManager(DatabaseManager::Instance())
{
    isRunning = false;
}

void Server::Start()
{
    if (listener.listen(LISTENER_PORT) != sf::Socket::Status::Done)
    {
        std::cerr << "Failed to bind listener on port 55001" << std::endl;
        return;
    }

    std::cout << "Server launched at port: " << LISTENER_PORT << std::endl;

        // Setup own variables
    socketSelector.add(listener);
    isRunning = true;

        //Setup the managers
    packetManager.Init();
    databaseManager.ConnectDb();


    eventManager.Subscribe(DISCONNECT, [this](std::string guid, CustomPacket& customPacket) {
           
        HandleDisconnection(guid);

    });
}

void Server::Update()
{
    while (isRunning) 
    {
        // Wait till one socket is ready with a timeout of 0.1 s
        if (!socketSelector.wait(sf::seconds(0.1f)))
            continue;

        // listener check for entry connections hand handles it
        if (socketSelector.isReady(listener))
        {
            HandleNewConnection();
        }
        else
        {
            // If there is activity in client sockets we update them
            clientManager.UpdateClients(socketSelector);
        }
    }

    databaseManager.DisconnectDb();
}

void Server::HandleNewConnection()
{
    try {
        Client& newClient = clientManager.CreateClient();

        if (listener.accept(newClient.GetSocket()) == sf::Socket::Status::Done)
        {
            newClient.GetSocket().setBlocking(false);
            socketSelector.add(newClient.GetSocket());
        }
    } catch (const std::exception& e) {
        std::cerr << "Error while handling new Connection: " << e.what() << std::endl;
    }
}

void Server::HandleDisconnection(const std::string& guid)
{
    std::function<void(const std::shared_ptr<Client>&)> removeSocket = [this](const std::shared_ptr<Client>& client) {
        socketSelector.remove(client->GetSocket());
        std::cout << "Socket from: " << client->GetSocket().getRemoteAddress().value() << " erased in socketSelector" << std::endl;
    };

    std::shared_ptr<Client> client;

    if (client = clientManager.GetAuthoritedClientById(guid))
        removeSocket(client);
    else if (client = clientManager.GetPendingClientByGuid(guid))
        removeSocket(client);
    else
        std::cerr << "Trying to disconnect non-existing client ( guid = " << guid << ")" << std::endl;

    clientManager.EraseClient(guid);

    std::cout << std::endl << std::endl << client->GetIsInRoom() << std::endl << std::endl;

    /*if (client->GetIsInRoom())
        ROOM_MANAGER.LeaveRoom(client->GetCurrentRoomId(), client);*/
}

Server::~Server()
{
}
