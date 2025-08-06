#include <cstdio>
#include <stdexcept>
#include <arpa/inet.h>

#include "util/util.hpp"

std::array<char, INET6_ADDRSTRLEN> getIPstring(const sockaddr* address)
{
	if(address->sa_family == AF_INET)
	{
		const void* ipv4 {&((sockaddr_in*)address)->sin_addr};

		std::array<char, INET6_ADDRSTRLEN> ipString;
		inet_ntop(AF_INET, ipv4, ipString.data(), ipString.size());

		return ipString;
	}
	else if(address->sa_family == AF_INET6)
	{
		const void* ipv6 {&((sockaddr_in6*)address)->sin6_addr};

		std::array<char, INET6_ADDRSTRLEN> ipString;
		inet_ntop(AF_INET6, ipv6, ipString.data(), ipString.size());

		return ipString;
	}
}

int getPort(const sockaddr* address)
{
	if(address->sa_family == AF_INET)
	{
		return ntohs(((sockaddr_in*)address)->sin_port);
	}
	else if(address->sa_family == AF_INET6)
	{
		return ntohs(((sockaddr_in6*)address)->sin6_port);
	}
}

void recvFull(uint64_t socket, char* buffer, int recvLength, int flags)
{
	try
	{
		for(int recved {0}; recved != recvLength; )
		{
			int currentRecved {sock::recv(socket, buffer + recved, recvLength - recved, flags)};

			if(currentRecved == 0)
			{
				throw DisconnectFlag {}; 
			}

			recved += currentRecved;
		}
	}
	catch(int errorCode)
	{
		if(errorCode == ECONNRESET)
		{
			throw DisconnectFlag {};
		}
		else
		{
			throw;
		}
	}
}

void sendFull(uint64_t socket, const char* buffer, int sendLength, int flags)
{
	for(int sent {0}; sent != sendLength; )
	{
		sent += sock::send(socket, buffer + sent, sendLength - sent, flags);
	}
}

uint16_t recvUint16(uint64_t socket)
{
	uint16_t num;
	recvFull(socket, (char*)&num, sizeof(num), 0);
	
	return ntohs(num);
}

void sendUint16(uint64_t socket, uint16_t num)
{
	num = htons(num);
	sendFull(socket, (char*)&num, sizeof(num), 0);
}