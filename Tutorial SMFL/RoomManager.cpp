#include "RoomManager.h"

RoomManager::RoomManager() : roomSize(4) {}

RoomManager& RoomManager::Instance()
{
	static RoomManager instance;
	return instance;
}

bool RoomManager::CreateRoom(const std::string& roomId, std::shared_ptr<Client> client)
{
	auto roomIt = FindRoomById(roomId);

	if (roomIt == rooms.end()) // There isn't a room with the id, then we create it
	{
		std::shared_ptr<Room> room = std::make_shared<Room>(roomId);
		rooms.push_back(room);
		room->AddClient(client);
		std::cout << rooms.size();
		return true;
	}

	return false; // There exists a room with this id
}

bool RoomManager::JoinRoom(const std::string& roomId, std::shared_ptr<Client> client)
{
	auto roomIt = FindRoomById(roomId);

	if (roomIt != rooms.end())
	{
		roomIt->get()->AddClient(client);
		std::cout << "Client " << client->GetUsername() << " joined room with id: " << roomId << std::endl;
		return true;
	}
	else
	{
		std::cerr << "Room with ID " << roomId << " not found." << std::endl;
		return false;
	}
}

void RoomManager::LeaveRoom(const std::string& roomId, std::shared_ptr<Client> client)
{
	auto roomIt = FindRoomById(roomId);

	if (roomIt != rooms.end())
	{
		roomIt->get()->RemoveClient(client);
	}
	else
	{
		std::cerr << "Room with ID " << roomId << " not found." << std::endl;
	}
}

void RoomManager::DeleteRoom(const std::string& roomId)
{
	auto roomIt = FindRoomById(roomId);

	if (roomIt != rooms.end())
	{
		rooms.erase(roomIt);
		std::cout << "Room with ID " << roomId << " deleted." << std::endl;
	}
	else
	{
		std::cerr << "Room with ID " << roomId << " not found." << std::endl;
	}
}

Room* RoomManager::GetFullRoom()
{
	auto roomIt = std::find_if(rooms.begin(), rooms.end(),
		[this](const std::shared_ptr<Room>& room) {
			return room->GetClients().size() >= roomSize;
		});

	if (roomIt != rooms.end())
	{
		std::cout << "Full room found with ID: " << (*roomIt)->GetId() << std::endl;
		return roomIt->get();
	}
	else
	{
		std::cerr << "No full room found." << std::endl;
		return nullptr;
	}
}

std::vector<std::shared_ptr<Room>>::iterator RoomManager::FindRoomById(const std::string& roomId)
{
	auto roomIt = std::find_if(rooms.begin(), rooms.end(),
		[roomId](const std::shared_ptr<Room>& room) {
			return room->GetId() == roomId;
		});

	if (roomIt != rooms.end())
		std::cout << "Room with ID " << roomId << " found." << std::endl;
	else
		std::cerr << "Room with ID " << roomId << " not found." << std::endl;

	return roomIt;
}
