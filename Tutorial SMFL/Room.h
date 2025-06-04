#pragma once
#include <vector>
#include "Client.h"
#include <memory>
#include <iostream>
#include <mutex>

class Room
{
private:
	std::string id;
	bool isClosed;
	std::vector<std::shared_ptr<Client>> clients;	

	std::mutex roomMutex;
public:

	Room() = default;
	Room(const std::string id);
	~Room();

	void AddClient(std::shared_ptr<Client> client);
	void RemoveClient(std::shared_ptr<Client> client);

	void CheckIfRoomFull(std::shared_ptr<Client> client);
	void CheckIfRoomEmpty(const std::string id);

	inline std::string GetId() const { return id; }
	inline bool GetIsClosed() const { return isClosed; }
	inline void SetIsClosed(const bool value) { isClosed = value; }
	inline void SetId(const std::string id) { this->id = id; }
	inline std::vector<std::shared_ptr<Client>> GetClients() const { return clients; }

};