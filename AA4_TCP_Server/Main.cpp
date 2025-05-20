#include <SFML/Network.hpp>
#include "Server.h"
#include <iostream>
// ----------------------------------- Server -----------------------------------

int main()
{
	try {
		Server server;
		server.Start();
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

