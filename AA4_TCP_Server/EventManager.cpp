
#include "EventManager.h"
#include <iostream>

void EventManager::Subscribe(const PacketType type, Callback _callback)
{
    subscribers[type].push_back(_callback);
}

void EventManager::Emit(const PacketType type, std::string _guid, CustomPacket customPacket)
{

    std::unordered_map<PacketType, std::vector<Callback>>::iterator it = subscribers.find(type);

    if (it != subscribers.end()) {
        for (Callback& callback : it->second) {
            callback(_guid, customPacket);
        }
    }
    else
    {
        std::cerr << "The event: " << type << " does not exists" << std::endl;
    }
}

EventManager& EventManager::Instance()
{
    static EventManager instance;
    return instance;
}