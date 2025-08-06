#include <thread>

#include "client_room.hpp"

int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		printf("Usage : [client_binary_name] [server_ip] [server_port] [username]");
		return 1;
	}
	
	std::string_view ip {argv[1]};
	std::string_view port {argv[2]};
	std::string_view username {argv[3]};

	Client client {ip, port, username};

	std::thread sendThread {&Client::sendThread, &client};
	std::thread recvThread {&Client::recvThread, &client};

	recvThread.join();
	sendThread.join();

	return 0;
}