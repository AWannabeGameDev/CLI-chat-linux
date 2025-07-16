#include <stdio.h>
#include <format>
#include <unistd.h>

#include <util/util.hpp>
#include <util/wsa_wrapper.hpp>
#include <util/constants.hpp>
#include <util/protocol_structs.hpp>

#include "server_room.hpp"

Server::Server(std::string_view serviceName)
{
	addrinfo hints {};
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	addrinfo* myAddress;
	wsa::getAddrInfo(nullptr, serviceName.data(), &hints, &myAddress);

	_listenPoll.fd = wsa::socket(myAddress->ai_family, myAddress->ai_socktype, myAddress->ai_protocol);
	_listenPoll.events = POLLIN;
	wsa::bind(_listenPoll.fd, myAddress->ai_addr, (int)myAddress->ai_addrlen);
	wsa::listen(_listenPoll.fd, 10);

	printf("Listening for connection requests on %s:%d...\n",
			getIPstring(myAddress->ai_addr).data(), getPort(myAddress->ai_addr));

	freeaddrinfo(myAddress);
}

Server::~Server()
{
	shutdown(_listenPoll.fd, SHUT_RDWR);
	close(_listenPoll.fd);
	
	for(auto& clientPoll : _clientPolls)
	{
		shutdown(clientPoll.fd, SHUT_RDWR);
		close(clientPoll.fd);
	}
}

void Server::connectionThread()
{
	while(!_shutdown)
	{
		int requested {poll(&_listenPoll, 1, POLL_TIMEOUT)};

		if(!(requested && (_listenPoll.revents & POLLIN)))
		{
			continue;	
		}

		std::lock_guard dataLock {_dataMutex};

		sockaddr_storage newClientAddress;
		sockaddr* newClientAddressPtr {(sockaddr*)&newClientAddress};
		socklen_t newClientAddrLen {sizeof(newClientAddress)};

		uint64_t newClientSocket {wsa::accept(_listenPoll.fd, newClientAddressPtr, &newClientAddrLen)};
		setsockopt(newClientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&POLL_TIMEOUT, sizeof(POLL_TIMEOUT));

		Username& username {_clientUsernames.emplace_back()};

		try
		{
			printf("Receiving username length...\n");

			username.nntLength = recvUint16(newClientSocket);
			if(username.nntLength > MAX_NNT_USERNAME_SIZE)
			{
				throw DisconnectFlag {};
			}

			printf("Receiving username...\n");

			recvFull(newClientSocket, username.ntText.data(), username.nntLength, 0);
		}
		catch(const DisconnectFlag&)
		{
			shutdown(newClientSocket, SHUT_RDWR);
			close(newClientSocket);

			_clientUsernames.pop_back();
			continue;
		}

		_clientPolls.emplace_back(newClientSocket, POLLIN | POLLHUP, 0);

		std::string connectText {std::format("{} connected.", username.ntText.data())};

		ChatMessage& newMsg {_recvMsgs.emplace_back()};
		newMsg.username = Username {.nntLength {0}};
		newMsg.nntMsgLength = (uint16_t)connectText.size();
		std::copy(connectText.begin(), connectText.end(), newMsg.ntMsg.data());

		printf("New client %s from %s:%d connected.\n", 
				username.ntText.data(),
				getIPstring(newClientAddressPtr).data(), 
				getPort(newClientAddressPtr));
	}
}

void Server::chatThread()
{
	while(!_shutdown)
	{
		int socketActionCount {poll(_clientPolls.data(), _clientPolls.size(), POLL_TIMEOUT)};

		std::lock_guard dataLock {_dataMutex};

		for(size_t clientIdx {0}, actionIdx {0}; (clientIdx < _clientPolls.size()) && (actionIdx < socketActionCount); )
		{
			if(!(_clientPolls[clientIdx].revents & POLLIN))
			{
				clientIdx++;
				continue;
			}

			ChatMessage& newRecvMsg {_recvMsgs.emplace_back()};
			newRecvMsg.username = _clientUsernames[clientIdx];

			try
			{
				newRecvMsg.nntMsgLength = recvUint16(_clientPolls[clientIdx].fd);
				if(newRecvMsg.nntMsgLength > MAX_NNT_MSG_SIZE)
				{
					_recvMsgs.pop_back();

					clientIdx++;
					actionIdx++;
					continue;
				}
				
				recvFull(_clientPolls[clientIdx].fd, newRecvMsg.ntMsg.data(), newRecvMsg.nntMsgLength, 0);
				
				printf("[%s] %s\n", newRecvMsg.username.ntText.data(), newRecvMsg.ntMsg.data());

				clientIdx++;
				actionIdx++;
			}
			catch(const DisconnectFlag&)
			{
				_recvMsgs.pop_back();

				shutdown(_clientPolls[clientIdx].fd, SHUT_RDWR);
				close(_clientPolls[clientIdx].fd);

				std::string disconnectText {std::format("{} disconnected.", _clientUsernames[clientIdx].ntText.data())};

				ChatMessage& newMsg {_recvMsgs.emplace_back()};
				newMsg.username = Username {.nntLength {0}};
				newMsg.nntMsgLength = (uint16_t)disconnectText.size();
				std::copy(disconnectText.begin(), disconnectText.end(), newMsg.ntMsg.data());

				_clientPolls.erase(_clientPolls.begin() + clientIdx);
				_clientUsernames.erase(_clientUsernames.begin() + clientIdx);
			
				printf("%s\n", disconnectText.data());

				if(_clientPolls.empty())
				{
					printf("All clients disconnected.\n");
				}

				actionIdx++;
			}
		}

		if(_recvMsgs.empty())
		{
			continue;
		}

		for(const auto& clientPoll : _clientPolls)
		{
			sendUint16(clientPoll.fd, (uint16_t)_recvMsgs.size());

			for(const auto& msg : _recvMsgs)
			{
				sendUint16(clientPoll.fd, msg.username.nntLength);
				sendUint16(clientPoll.fd, msg.nntMsgLength);
				sendFull(clientPoll.fd, msg.username.ntText.data(), msg.username.nntLength, 0);
				sendFull(clientPoll.fd, msg.ntMsg.data(), msg.nntMsgLength, 0);
			}
		}

		_recvMsgs.clear();
	}
}