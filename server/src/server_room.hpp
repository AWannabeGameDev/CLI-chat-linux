#ifndef SERVER_ROOM_HPP
#define SERVER_ROOM_HPP

#include <cstdint>
#include <vector>
#include <mutex>
#include <sys/poll.h>

#include <util/terminal_text_box.hpp>
#include <util/sock_wrapper.hpp>
#include <util/protocol_structs.hpp>

class Server
{
private:
	pollfd _listenPoll {};
	std::vector<pollfd> _clientPolls {};
	std::vector<Username> _clientUsernames;

	std::vector<ChatMessage> _recvMsgs {};
	std::mutex _dataMutex {};

	bool _shutdown {false};

public:
	Server(std::string_view serviceName);
	~Server();

	void connectionThread();
	void chatThread();
};

#endif