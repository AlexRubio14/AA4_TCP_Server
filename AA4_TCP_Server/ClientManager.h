#include <vector>
#include "PacketManager.h"
#include <unordered_map>
#include "Client.h"

#define CLIENT_MANAGER ClientManager::Instance()


class ClientManager {
private:
    std::unique_ptr<sf::TcpSocket> socket;

    std::unordered_map<std::string,std::shared_ptr<Client>> authenticatedClients;
	std::vector<std::shared_ptr<Client>> pendingClients; // Not logged

    ClientManager() = default;
    ClientManager(const ClientManager&) = delete;
    ClientManager& operator=(const ClientManager&) = delete;

    int temporaryGuidCount = 1;

public:

    static ClientManager& Instance();

    explicit ClientManager(std::unique_ptr<sf::TcpSocket> clientSocket);

    Client& CreateClient();

    void DisconnectClient(std::string guid);

    std::string CreateGuid(Client& client);

    std::string CreateTemporaryGuid();

    void UpdateClients(sf::SocketSelector& _socketSelector);

    std::string PromoteClientToAuthenticated(const std::string guid, const std::string username);
    void InitAuthenticatedClient(Client& client, const std::string username);

    std::shared_ptr<Client> GetAuthoritedClientById(const std::string guid);
    bool CheckIfUserAlreadyLogged(const std::string& username) const;
    std::shared_ptr<Client> GetPendingClientById(const std::string guid);
};