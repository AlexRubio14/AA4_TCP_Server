#pragma once
#include <string>
#include <SFML/Network.hpp>
#include "ClientState.h"

class Client
{
private:

	std::string guid;
	std::string username;
	std::unique_ptr<sf::TcpSocket> socket;
	int port;
	ClientState clientState;

	bool isInRoom;
	std::string currentRoomId;

public:

	Client();
	~Client();

	void HandleIncomingPackets();

	inline std::string GetGuid() const  { return guid; }
	inline std::string GetUsername() const { return username; } 
	inline sf::TcpSocket& GetSocket() { return *socket; }
	inline int GetPort() const { return port; }
	inline bool GetIsInRoom() const { return isInRoom; }
	inline std::string GetCurrentRoomId() const { return currentRoomId; }

	inline void SetGuid(const std::string _guid) { guid = _guid; }
	inline void SetUsername(const std::string& _username) { username = _username; }
	inline void SetPort(const int portValue) { port = portValue; }
	inline void SetState(const ClientState _clientState) { clientState = _clientState; }
	inline void SetIsInRoom(const bool value) { isInRoom = value; }
	inline void SetCurrentRoomId(const std::string _roomId) { currentRoomId = _roomId; }
};

