#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <util/util.hpp>
#include <util/wsa_wrapper.hpp>
#include <util/protocol_structs.hpp>

#include "client_room.hpp"

Client::Client(std::string_view serverAddress, std::string_view serviceName, std::string_view username)
{
	addrinfo hints {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	addrinfo* serverAddrInfo;
	wsa::getAddrInfo(serverAddress.data(), serviceName.data(), &hints, &serverAddrInfo);

	uint64_t socket {wsa::socket(serverAddrInfo->ai_family, serverAddrInfo->ai_socktype, serverAddrInfo->ai_protocol)};

	printf("Connecting to server at %s:%d...\n",
		getIPstring(serverAddrInfo->ai_addr).data(), getPort(serverAddrInfo->ai_addr));

	wsa::connect(socket, serverAddrInfo->ai_addr, (int)serverAddrInfo->ai_addrlen);

	sendUint16(socket, (uint16_t)username.size());
	sendFull(socket, username.data(), (int)username.size(), 0);

	printf("Connected to server successfully.\n");
	printf("Send '!exit' to close the chat.\n");

	_socketPoll.fd = socket;
	_socketPoll.events = POLLIN | POLLHUP;

	freeaddrinfo(serverAddrInfo);
}

Client::~Client()
{
	shutdown(_socketPoll.fd, SHUT_RDWR);
	close(_socketPoll.fd);
}

void Client::sendThread()
{
	while(!_shutdown)
	{
		std::string_view line {_textBox.getLine()};

		if(line == "!exit")
		{
			_shutdown = true;
			break;
		}

		if(!_peerClosed)
		{
			sendUint16(_socketPoll.fd, (uint16_t)line.size());
			sendFull(_socketPoll.fd, line.data(), (int)line.size(), 0);
		}

		std::lock_guard<std::mutex> dataLock {_consoleMutex};
		_textBox.erase();
	}
}

void Client::recvThread()
{
	while(!(_shutdown || _peerClosed))
	{
		int socketAction {poll(&_socketPoll, 1, POLL_TIMEOUT)};

		if(!(socketAction && (_socketPoll.revents & POLLIN)))
		{
			continue;
		}

		std::lock_guard<std::mutex> dataLock {_consoleMutex};

		try
		{
			uint16_t msgCount {recvUint16(_socketPoll.fd)};

			_textBox.erase();

			for(int i {0}; i < msgCount; i++)
			{
				ChatMessage msg {};

				msg.username.nntLength = recvUint16(_socketPoll.fd);
				msg.nntMsgLength = recvUint16(_socketPoll.fd);

				recvFull(_socketPoll.fd, msg.username.ntText.data(), msg.username.nntLength, 0);
				recvFull(_socketPoll.fd, msg.ntMsg.data(), msg.nntMsgLength, 0);

				printf("[%s] %s\n", msg.username.ntText.data(), msg.ntMsg.data());
			}

			_textBox.redraw();
		}
		catch(const DisconnectFlag&)
		{
			_peerClosed = true;

			_textBox.erase();
			printf("Connection closed by server.\n");
			_textBox.redraw();
		}
	}
}