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

std::shared_ptr<Client> ClientManager::CreatePendingClient()
{
	std::shared_ptr<Client> client = std::make_shared<Client>();

	client->SetGuid(CreateTemporaryGuid());
	client->SetState(PENDING);

	pendingClients.emplace(client->GetGuid(), client);
	std::cout << "New client created and added to pendingClients" << std::endl;

	return client;
}

void ClientManager::EraseClient(const std::string& guid)
{
	if (authenticatedClientsByGuid.erase(guid)) {
		std::cout << "Client with guid: " << guid << "erased from atuhentichatedClients" << std::endl;
		return;
	}
	else if (pendingClients.erase(guid)) {
		std::cout << "Client with guid: " << guid << "erased from pendingClients" << std::endl;
		return;
	}
	else
		std::cerr << "Client with guid: " << guid << "not found in the stl structures" << std::endl;

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

int ClientManager::TakeNextClientId()
{
	return nextClientId++;
}

void ClientManager::UpdateClients(sf::SocketSelector& _socketSelector)
{
	for (auto& [id, client] : pendingClients)
	{
		if (_socketSelector.isReady(client->GetSocket()))
			client->HandleIncomingPackets();
	}

	//Structure Binding improved in c++ v17 and considered best practice in modern c++, 
	// auto is compulsory using this feature because the linker needs to deduct the type of the variable

	for (auto& [id, client] : authenticatedClientsByGuid)
	{
		if (_socketSelector.isReady(client->GetSocket()))
			client->HandleIncomingPackets();
	}
}

std::string ClientManager::PromoteClientToAuthenticated(const std::string guid, const std::string username)
{
	std::shared_ptr<Client> client = GetPendingClientByGuid(guid);

	if (!client)
	{
		std::cerr << "Client with guid: " << guid << "not found in pending clients" << std::endl;
		return " ";
	}

	InitAuthenticatedClient(*client, username);

	authenticatedClientsByGuid[client->GetGuid()] = client;
	pendingClients.erase(guid);

	std::cout << "Client with TemporaryGUID " << guid << " promoted to authenticated." << std::endl << authenticatedClientsByGuid[client->GetGuid()].get()->GetGuid() << std::endl;

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
	std::unordered_map<std::string, std::shared_ptr<Client>>::iterator it = authenticatedClientsByGuid.find(guid);

	if (it != authenticatedClientsByGuid.end())
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
	const auto& clientIt = std::find_if(authenticatedClientsByGuid.begin(), authenticatedClientsByGuid.end(),
		[username](const std::pair<const std::string, std::shared_ptr<Client>> pair) {
			const std::string currentUsername = pair.second->GetUsername();
			if (currentUsername == username)
			{
				std::cerr << "Client with username: " << username << " is already logged" << std::endl;
				return true;
			}
			return false;
		});

	if (clientIt != authenticatedClientsByGuid.end())
		return true;

	std::cerr << "Client with username: " << username << " not logged yet" << std::endl;
	return false;
}

std::shared_ptr<Client> ClientManager::GetPendingClientByGuid(const std::string guid)
{
	std::unordered_map<std::string, std::shared_ptr<Client>>::iterator it = pendingClients.find(guid);

	if (it != pendingClients.end())
	{
		std::cout << "Client with GUID " << guid << " found in pendingClients Map" << std::endl;
		return it->second;
	}
	else
	{
		std::cerr << "Client with GUID " << guid << " not found in pendingClients Map" << std::endl;
		return nullptr;
	}
}


