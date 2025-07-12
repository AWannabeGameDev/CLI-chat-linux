#include <thread>

#include "client_room.hpp"

int main(int argc, char* argv[])
{
	std::string_view ip;
	std::string_view port;
	std::string_view username;

	if(argc != 4)
	{
		ip = std::string_view {"192.168.1.100"};
		port = std::string_view {"3490"};
		username = std::string_view {"New Client"};
	}
	else
	{
		ip = std::string_view {argv[1]};
		port = std::string_view {argv[2]};
		username = std::string_view {argv[3]};
	}

	Client client {ip, port, username};

	std::thread sendThread {&Client::sendThread, &client};
	std::thread recvThread {&Client::recvThread, &client};

	recvThread.join();
	sendThread.join();

	return 0;
}