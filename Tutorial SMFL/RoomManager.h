#pragma once
#include <vector>
#include <memory>
#include "Room.h"
#include "Client.h"

#define ROOM_MANAGER RoomManager::Instance()

class RoomManager
{
private:
	RoomManager();
	RoomManager(const RoomManager&) = delete;
	RoomManager& operator=(const RoomManager&) = delete;

	std::vector<std::shared_ptr<Room>> rooms;
	int roomSize;
public:
	static RoomManager& Instance();
	bool CreateRoom(const std::string& roomId, std::shared_ptr<Client> client);
	bool JoinRoom(const std::string& roomId, std::shared_ptr<Client> client);
	void LeaveRoom(const std::string& roomId, std::shared_ptr<Client> client);
	void DeleteRoom(const std::string& roomId);
	Room* GetFullRoom();
	std::vector<std::shared_ptr<Room>>::iterator FindRoomById(const std::string& roomId);

	inline std::vector<std::shared_ptr<Room>> GetRooms() const { return rooms; }
	inline int GetRoomSize() const { return roomSize; }
};

