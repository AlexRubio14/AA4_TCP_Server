#include "Room.h"
#include "EventManager.h"
#include "RoomManager.h"


Room::Room(const std::string id)
{
	this->id = id;
	isClosed = false;
}

Room::~Room()
{
	std::cout << "Room erased" << std::endl;
}

void Room::AddClient(std::shared_ptr<Client> client)
{
	clients.push_back(client);
	std::cout << "Client added to room" << std::endl;
}

void Room::RemoveClient(std::shared_ptr<Client> client)
{
	auto clientsIt = std::remove(clients.begin(), clients.end(), client);
	if (clientsIt != clients.end())
	{
		clients.erase(clientsIt, clients.end());
		std::cout << "Client: " << client->GetGuid() << " removed from room" << std::endl;

		CheckIfRoomEmpty(id);
	}
	else
		std::cerr << "Client not found in room" << std::endl;
}

void Room::CheckIfRoomFull(std::shared_ptr<Client> client)
{
	std::lock_guard<std::mutex> lock(roomMutex);

	if (!isClosed && clients.size() >= ROOM_MANAGER.GetRoomSize())
	{
		std::cout << "Room is full" << std::endl;
		CustomPacket customPacket;
		std::cout << "Starting game" << std::endl;
		isClosed = true;
		EVENT_MANAGER.Emit(START_GAME, client.get()->GetGuid(), customPacket);
	}

	std::cout << "The size of the room is: " << clients.size() << std::endl;
}

void Room::CheckIfRoomEmpty(const std::string id)
{
	std::cout << std::endl << std::endl << clients.size() << std::endl << std::endl;
	if (clients.size() == 0)
	{
		std::cout << "Room is empty" << std::endl;
		auto rooms = ROOM_MANAGER.GetRooms();
		std::cout <<  rooms .size() << std::endl;
		ROOM_MANAGER.DeleteRoom(id);
	}
}
