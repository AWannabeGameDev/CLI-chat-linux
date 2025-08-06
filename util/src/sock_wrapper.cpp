#include <cstdio>
#include <cstdint>
#include <errno.h>

#include "util/sock_wrapper.hpp"

static void throwError(const char* errorStr)
{
	int error {errno};
	fprintf(stderr, "%s; Error-code : %d\n", errorStr, error);
	throw error;
}

void sock::getAddrInfo(const char* nodeName, const char* serviceName, const addrinfo* hints, addrinfo** result)
{
	int returnCode {::getaddrinfo(nodeName, serviceName, hints, result)};

	if(returnCode != 0)
	{
		throwError("Failed to retrieve address info");
	}
}

uint64_t wsa::socket(int af, int type, int protocol)
{
	uint64_t socketFD {::socket(af, type, protocol)};

	if(socketFD == -1)
	{
		throwError("Failed to create socket");
	}

	return socketFD;
}

void wsa::bind(uint64_t s, const sockaddr* name, int namelen)
{
	int returnCode {::bind(s, name, namelen)};

	if(returnCode == -1)
	{
		throwError("Failed to bind socket to the specified port");
	}
}

void wsa::listen(uint64_t s, int backlog)
{
	int returnCode {::listen(s, backlog)};

	if(returnCode == -1)
	{
		throwError("Failed to initiate listening on socket");
	}
}

uint64_t wsa::accept(uint64_t s, sockaddr* addr, socklen_t* addrlen)
{
	uint64_t socketFD {::accept(s, addr, addrlen)};

	if(socketFD == -1)
	{
		throwError("Failed to accept connection request");
	}

	return socketFD;
}

int wsa::send(uint64_t s, const char* buf, int len, int flags)
{
	int returnCode {::send(s, buf, len, flags)};

	if(returnCode == -1)
	{
		throwError("Failed to send data through the specified socket");
	}

	return returnCode;
}

void wsa::connect(uint64_t s, const sockaddr* name, int namelen)
{
	int returnCode {::connect(s, name, namelen)};

	if(returnCode == -1)
	{ 
		throwError("Failed to connect to specified address");
	}
}

int wsa::recv(uint64_t s, char* buf, int len, int flags)
{
	int returnCode {::recv(s, buf, len, flags)};

	if(returnCode == -1)
	{
		throwError("Failed to receive data from the specified socket");
	}

	return returnCode;
}