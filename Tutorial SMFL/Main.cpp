#include <SFML/Network.hpp>
#include "Server.h"
#include "DatabaseManager.h"
// ----------------------------------- Server -----------------------------------

void main()
{
	Server server;
	server.Start();
	server.Update();
}

