#include <thread>

#include "server_room.hpp"

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Usage : [server_binary_name] [port]");
	}

	std::string_view port {argv[1]};

	Server server {port};

	std::thread connectionThread {&Server::connectionThread, &server};
	std::thread chatThread {&Server::chatThread, &server};

	connectionThread.join();
	chatThread.join();

	return 0;
}