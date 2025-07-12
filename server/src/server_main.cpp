#include <thread>

#include "server_room.hpp"

int main(int argc, char* argv[])
{
	std::string_view port;

	if(argc != 2)
	{
		port = std::string_view {"3490"};
	}
	else
	{
		port = std::string_view {argv[1]};
	}

	Server server {port};

	std::thread connectionThread {&Server::connectionThread, &server};
	std::thread chatThread {&Server::chatThread, &server};

	connectionThread.join();
	chatThread.join();

	return 0;
}