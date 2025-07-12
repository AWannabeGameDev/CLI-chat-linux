#ifndef WSA_WRAPPER_HPP
#define WSA_WRAPPER_HPP

#include <sys/socket.h>
#include <netdb.h>

namespace wsa
{
	void getAddrInfo(const char* pNodeName, const char* pServiceName, const addrinfo* pHints, addrinfo** ppResult);
	uint64_t socket(int af, int type, int protocol);
	void bind(uint64_t s, const sockaddr* name, int namelen);
	void connect(uint64_t s, const sockaddr* name, int namelen);
	void listen(uint64_t s, int backlog);
	uint64_t accept(uint64_t s, sockaddr* addr, socklen_t* addrlen);
	int send(uint64_t s, const char* buf, int len, int flags);
	int recv(uint64_t s, char* buf, int len, int flags);

	// shutdown
	// poll
};

#endif