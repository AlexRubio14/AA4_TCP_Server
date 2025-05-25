#include <unordered_map>
#include <memory>
#include <string>
#include "Client.h"

#define CLIENT_MANAGER ClientManager::Instance()


class ClientManager {
private:
   

    ClientManager() = default;
    ClientManager(const ClientManager&) = delete;
    ClientManager& operator=(const ClientManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<Client>> authenticatedClientsByGuid;
    std::unordered_map<uint32_t, std::shared_ptr<Client>> authenticatedClientsById;
    std::unordered_map<std::string, std::shared_ptr<Client>> pendingClients; // Not logged

    int temporaryGuidCount = 1;
    uint32_t nextClientId = 1;

public:

    static ClientManager& Instance();

    std::shared_ptr<Client> CreatePendingClient();

    std::string PromoteClientToAuthenticated(const std::string guid, const std::string username);

    void EraseClient(std::string& guid);

    std::string CreateGuid(Client& client);

    std::string CreateTemporaryGuid();

    void UpdateClients(sf::SocketSelector& _socketSelector);

    void InitAuthenticatedClient(Client& client, const std::string username);

    std::shared_ptr<Client> GetAuthoritedClientById(const std::string guid);
    bool CheckIfUserAlreadyLogged(const std::string& username) const;
    std::shared_ptr<Client> GetPendingClientByGuid(const std::string guid);
};