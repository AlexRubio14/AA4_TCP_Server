#include "PacketManager.h"
#include <iostream>
#include "EventManager.h"
#include "DatabaseManager.h"
#include "ClientManager.h"
#include <fstream>
#include <sstream>

void PacketManager::HandleHandshake(sf::Packet& packet)
{
	std::string messageFromClient;
	packet >> messageFromClient;

	std::cout << "Messages received from client: " << messageFromClient << std::endl;
}

void PacketManager::SendHandshake(const std::string guid)
{
	std::string responseMessage = "Hello client, i'm the server";
	CustomPacket responsePacket(PacketType::HANDSHAKE);

	responsePacket.packet << responseMessage;

	std::shared_ptr<Client> client = CLIENT_MANAGER.GetPendingClientByGuid(guid);
	if (client != nullptr)
	{
		SendPacketToClient(client, responsePacket);
		std::cout << "Message send to client: " << responseMessage << std::endl;
	}
}

PacketManager& PacketManager::Instance()
{
	static PacketManager instance;
	return instance;
}

void PacketManager::Init()
{
	EVENT_MANAGER.Subscribe(PacketType::HANDSHAKE, [this](std::string guid, CustomPacket& customPacket) {
		HandleHandshake(customPacket.packet);
		SendHandshake(guid);
	});

	EVENT_MANAGER.Subscribe(PacketType::LOGIN, [this](std::string guid, CustomPacket& customPacket) {

		std::string username;
		std::string password;
		customPacket.packet >> username >> password;

		std::string message;
		CustomPacket responsePacket;

		switch (DB_MANAGER.ValidateUser(username, password))
		{
		case LoginResult::SUCCESS: {

			message = "User logged in successfully";
			responsePacket.packet << PacketType::LOGIN_SUCCESS << message;

			std::string authGuid = CLIENT_MANAGER.PromoteClientToAuthenticated(guid, username);
			EVENT_MANAGER.Emit(PacketType::LOGIN_SUCCESS, authGuid, responsePacket);
			std::cout << "User " << username << " logged in successfully and promoted to Authenticated" << std::endl;
			break;
		}
		case LoginResult::INVALID_CREDENTIALS:

			message = "Credentials used for login are invalid";
			responsePacket.packet << PacketType::LOGIN_ERROR << message;

			EVENT_MANAGER.Emit(PacketType::LOGIN_ERROR, guid, responsePacket);
			std::cout << "User " << username << " failed to log in because: " << message << std::endl;

			break;

		case LoginResult::USER_ALREADY_LOGGED:
			message = "The user is already logged";
			responsePacket.packet << PacketType::LOGIN_ERROR << message;
			EVENT_MANAGER.Emit(PacketType::LOGIN_ERROR, guid, responsePacket);
			std::cout << "User" << username << " failed to log in because: " << message << std::endl;
			break;
		case LoginResult::QUERY_ERROR:

			message = "Error querying the database";
			responsePacket.packet << PacketType::LOGIN_ERROR << message;

			EVENT_MANAGER.Emit(PacketType::LOGIN_ERROR, guid, responsePacket);
			std::cout << message << std::endl;
			break;
		default:
			break;
		}
	});

	EVENT_MANAGER.Subscribe(PacketType::LOGIN_SUCCESS, [this](std::string guid, CustomPacket& customPacket) {

		std::shared_ptr<Client> client = CLIENT_MANAGER.GetAuthoritedClientById(guid);

		if (client)
			SendPacketToClient(client, customPacket);

	});

	EVENT_MANAGER.Subscribe(PacketType::LOGIN_ERROR, [this](std::string guid, CustomPacket& customPacket) {

		std::shared_ptr<Client> client = CLIENT_MANAGER.GetPendingClientByGuid(guid);

		if (client)
			SendPacketToClient(client, customPacket);
	});

	EVENT_MANAGER.Subscribe(PacketType::REGISTER, [](std::string guid, CustomPacket& customPacket) {
		std::string username;
		std::string password;
		customPacket.packet >> username >> password;

		std::string message;
		CustomPacket responsePacket;

		switch (DB_MANAGER.CreateUser(username, password))
		{
		case RegisterResult::SUCCESS: {
			message = "User registered successfully";
			responsePacket.packet << PacketType::REGISTER_SUCCESS << message;


			std::string authGuid = CLIENT_MANAGER.PromoteClientToAuthenticated(guid, username);
			EVENT_MANAGER.Emit(PacketType::REGISTER_SUCCESS, authGuid, responsePacket);
			std::cout << "User " << username << " registered successfully nad promoted to authenticated" << std::endl;
			break;
		}
		case RegisterResult::USERNAME_TAKEN:

			message = "Username already taken";
			responsePacket.packet << PacketType::REGISTER_ERROR << message;

			EVENT_MANAGER.Emit(PacketType::REGISTER_ERROR, guid, responsePacket);
			std::cout << "User " << username << " failed to register because: " << message << std::endl;
			break;
		case RegisterResult::QUERY_ERROR:

			message = "Error querying the database";
			responsePacket.packet << PacketType::REGISTER_ERROR << message;

			EVENT_MANAGER.Emit(PacketType::REGISTER_ERROR, guid, responsePacket);
			std::cout << "User " << username << " failed to register because: " << message << std::endl;
			break;
		case RegisterResult::INSERT_FAILED:

			message = "Error inserting the user into the database";
			responsePacket.packet << PacketType::REGISTER_ERROR << message;

			EVENT_MANAGER.Emit(PacketType::REGISTER_ERROR, guid, responsePacket);
			std::cout << "User " << username << " failed to register because: " << message << std::endl;
			break;
		default:
			break;
		}
	});

	EVENT_MANAGER.Subscribe(PacketType::REGISTER_SUCCESS, [this](std::string guid, CustomPacket& customPacket) {

		std::shared_ptr<Client> client = CLIENT_MANAGER.GetAuthoritedClientById(guid);

		if (client)
			SendPacketToClient(client, customPacket);
	});

	EVENT_MANAGER.Subscribe(PacketType::REGISTER_ERROR, [this](std::string guid, CustomPacket& customPacket) {

		std::shared_ptr<Client> client = CLIENT_MANAGER.GetPendingClientByGuid(guid);

		if (client)
			SendPacketToClient(client, customPacket);
	});

	
	EVENT_MANAGER.Subscribe(PacketType::START_QUEUE, [this](std::string guid, CustomPacket& customPacket) {
		std::shared_ptr<Client> client = CLIENT_MANAGER.GetAuthoritedClientById(guid);
		if (client)
		{
			// Enqueue the player in the matchmaking queue
			matchmakingManager->EnqueuePlayer(client);
			CustomPacket responsePacket(PacketType::START_QUEUE_RESULT);
			responsePacket.packet << "You have entered in queue";
			SendPacketToClient(client, responsePacket);
		}
	});

	EVENT_MANAGER.Subscribe(PacketType::CANCEL_QUEUE, [this](std::string guid, CustomPacket& customPacket) {
		std::shared_ptr<Client> client = CLIENT_MANAGER.GetAuthoritedClientById(guid);
		if (client)
		{
			// Here you would handle the logic for canceling a queue
			std::cout << "Client " << client->GetUsername() << " has canceled the queue" << std::endl;
			// You can send a response back to the client if needed
			bool result = matchmakingManager->DequeuePlayer(client);
			CustomPacket responsePacket(PacketType::CANCEL_QUEUE_RESULT);

			if (result)
				responsePacket.packet << "You have canceled the queue succesfully";
			else
				responsePacket.packet << "You do not have canceled the queue succesfully";

			SendPacketToClient(client, responsePacket);
		}
	});

	EVENT_MANAGER.Subscribe(PacketType::ASK_MAP, [this](std::string guid, CustomPacket& customPacket) {

		std::shared_ptr<Client> client = CLIENT_MANAGER.GetPendingClientByGuid(guid);

		if (client)
		{
			// Read and prepare the map file to send it to Client
			std::cout << "Client " << client->GetUsername() << " has requested a map." << std::endl;

			std::string jsonMap;
			try {

				std::ifstream mapFile("../Resources/map1.json");
				if (mapFile)
				{
					std::ostringstream buffer;
					buffer << mapFile.rdbuf();
					jsonMap = buffer.str();
				}
				else
				{
					std::cerr << "Error opening map file." << std::endl;
					return;
				}
			} 
			catch (const std::exception& e) {
				std::cerr << "Error reading map file: " << e.what() << std::endl;
				return;
			}

			// Create a response packet with the map data
			CustomPacket responsePacket(PacketType::RECEIVE_MAP);
			responsePacket.packet << jsonMap;
			SendPacketToClient(client, responsePacket);
		}
	});
}

void PacketManager::SetMatchMakingManager(MatchMakingManager& _matchmakingManager)
{
	matchmakingManager = &_matchmakingManager;
}

void PacketManager::ProcessPacket(const std::string& guid, CustomPacket& customPacket)
{
	customPacket.packet >> customPacket.type;

	std::cout << "Processing packet of type: " << static_cast<int>(customPacket.type) << std::endl;

	EVENT_MANAGER.Emit(customPacket.type, guid, customPacket);
}

void PacketManager::SendPacketToClient(const std::shared_ptr<Client>& client, CustomPacket& responsePacket)
{
	if (client->GetSocket().send(responsePacket.packet) == sf::Socket::Status::Done)
		std::cout << "Message sent to client " << std::endl;
	else
		std::cerr << "Error sending the message to client" << std::endl;
}
