#include "ClientManager.h"
#include <iostream>
#include "EventManager.h"
#include "PacketType.h"
#include "ClientState.h"
#include <chrono>
#include "SHA256.h"

ClientManager& ClientManager::Instance()
{
    static ClientManager instance;
    return instance;
}

ClientManager::ClientManager(std::unique_ptr<sf::TcpSocket> clientSocket)
    : socket(std::move(clientSocket)) 
{ 
    socket->setBlocking(false);
}

Client& ClientManager::CreateClient()
{
    std::shared_ptr<Client> client = std::make_shared<Client>();

	pendingClients.push_back(client);

	client->SetGuid(CreateTemporaryGuid());
	client->SetState(PENDING);
	std::cout << "New client created and added to pendingClients" << std::endl;

    return *client;
}

void ClientManager::DisconnectClient(std::string guid)
{
    auto authenticatedIt = authenticatedClients.find(guid);

    if (authenticatedIt != authenticatedClients.end()) {
        authenticatedClients.erase(authenticatedIt);
        std::cout << "Client removed from authenticatedClients" << std::endl;
        return;
    }

	for (auto pendingIt = pendingClients.begin(); pendingIt != pendingClients.end(); ++pendingIt) {
		if ((*pendingIt)->GetGuid() == guid) {
			pendingClients.erase(pendingIt);
			std::cout << "Client removed from pendingClients" << std::endl;
			return;
		}
	}
}

std::string ClientManager::CreateGuid(Client& client)
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	std::string stringGuid = client.GetUsername() + std::to_string(duration);

	SHA256 sha256;
	sha256.update(stringGuid);

	std::array<uint8_t, 32> hash = sha256.digest();


    return sha256.toString(hash);
}

std::string ClientManager::CreateTemporaryGuid()
{
	std::string guid = std::to_string(temporaryGuidCount);
	temporaryGuidCount++;
	std::cout << "Temporary GUID created: " << guid << std::endl;
	return guid;
}

void ClientManager::UpdateClients(sf::SocketSelector& _socketSelector)
{
	for (std::shared_ptr<Client> client : pendingClients)
	{
		if (client && _socketSelector.isReady(client->GetSocket()))
		{
			client->HandleIncomingPackets();
		}
		else if (!client)
		{
			std::cerr << "Invalid client pointer detected." << std::endl;
		}
	}
    
    //Structure Binding improved in c++ v17 and considered best practice in modern c++, 
    // auto is compulsory using this feature because the linker needs to deduct the type of the variable

	for (auto& [id, client] : authenticatedClients)
	{
        if (client && _socketSelector.isReady(client->GetSocket())) 
        {
            client->HandleIncomingPackets();
        }
        else if (!client)
        {
            std::cerr << "Invalid client pointer detected for client ID: " << id << std::endl;
        }
    }
}

std::string ClientManager::PromoteClientToAuthenticated(const std::string guid, const std::string username)
{
	std::shared_ptr<Client> client = GetPendingClientById(guid);

	InitAuthenticatedClient(*client.get(), username);

	authenticatedClients[client->GetGuid()] = client;
	pendingClients.erase(std::remove(pendingClients.begin(), pendingClients.end(), client), pendingClients.end());

	std::cout << "Client with TemporaryGUID " << guid << " promoted to authenticated." << std::endl << authenticatedClients[client->GetGuid()].get()->GetGuid() << std::endl;

	return client->GetGuid();
}

void ClientManager::InitAuthenticatedClient(Client& client, const std::string username)
{
    client.SetState(AUTHENTICATED);
	client.SetUsername(username);
    client.SetGuid(CreateGuid(client));
}

std::shared_ptr<Client> ClientManager::GetAuthoritedClientById(const std::string guid)
{
	std::unordered_map<std::string, std::shared_ptr<Client>>::iterator it = authenticatedClients.find(guid);

    if (it != authenticatedClients.end())
    {
        std::cout << "Client with GUID " << guid << " found in authenticatedClients Map" << std::endl;
        return it->second;
    }
    else
    {
        std::cerr << "Client with GUID " << guid << " not found in authenticatedClients Map" << std::endl;
        return nullptr;
    }
}

bool ClientManager::CheckIfUserAlreadyLogged(const std::string& username) const
{
	const auto& clientIt = std::find_if(authenticatedClients.begin(), authenticatedClients.end(),
		[username](const std::pair<const std::string, std::shared_ptr<Client>> pair) {
			const std::string currentUsername = pair.second->GetUsername();
			if (currentUsername == username)
			{
				std::cerr << "Client with username: " << username << " is already logged" << std::endl;
				return true;
			}
			return false;
		});

	if (clientIt != authenticatedClients.end())
		return true;

	std::cerr << "Client with username: " << username << " not logged yet" << std::endl;
	return false;
}

std::shared_ptr<Client> ClientManager::GetPendingClientById(const std::string guid)
{
    const auto clientIt = std::find_if(pendingClients.begin(), pendingClients.end(),
		[guid](const std::shared_ptr<Client>& client) {
			std::cerr << "Client with GUID " << guid << " found in pending clients Vector" << std::endl;

			return client->GetGuid() == guid;
	    });

    if(clientIt != pendingClients.end())
		return *clientIt;

    std::cerr << "Client with GUID " << guid << " not found in pending clients Vector" << std::endl;
    return nullptr;
}


