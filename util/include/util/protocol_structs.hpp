#ifndef PROTOCOL_STRUCTS_HPP
#define PROTOCOL_STRUCTS_HPP

#include <array>
#include <cstdint>

#include "util/constants.hpp"

struct Username
{
	uint16_t nntLength;
	std::array<char, MAX_NNT_USERNAME_SIZE + 1> ntText;
};

struct ChatMessage
{
	Username username;
	uint16_t nntMsgLength;
	std::array<char, MAX_NNT_MSG_SIZE + 1> ntMsg;
};

#endif