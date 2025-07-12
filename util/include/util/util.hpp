#ifndef UTIL_HPP
#define UTIL_HPP

#include <array>

#include "util/wsa_wrapper.hpp"

struct DisconnectFlag {};

std::array<char, INET6_ADDRSTRLEN> getIPstring(const sockaddr* address);
int getPort(const sockaddr* address);

void recvFull(uint64_t socket, char* buffer, int recvLength, int flags);
void sendFull(uint64_t socket, const char* buffer, int sendLength, int flags);

uint16_t recvUint16(uint64_t socket);
void sendUint16(uint64_t socket, uint16_t num);

#endif