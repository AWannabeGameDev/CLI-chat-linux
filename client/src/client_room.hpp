#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstdint>
#include <string_view>
#include <mutex>
#include <sys/poll.h>

#include <util/terminal_text_box.hpp>
#include <util/sock_wrapper.hpp>

class Client
{
private :
	pollfd _socketPoll;

	TerminalTextBox _textBox {};
	std::mutex _consoleMutex {};

	bool _shutdown {false};
	bool _peerClosed {false};

public :
	Client(std::string_view serverAddress, std::string_view serviceName, std::string_view username);
	~Client();

	void sendThread();
	void recvThread();
};

#endif