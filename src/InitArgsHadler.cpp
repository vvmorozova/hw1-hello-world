#include "InitArgsHandler.h"
#include <iostream>

void InitArgsHandler::checkArgsNumber()
{
	if (m_argc < 2) {
		throw std::logic_error("No arguments");
	}
}

int InitArgsHandler::getBlockLen()
{
	int blockLen = std::atoi(m_argv[1]);
	if (blockLen < 1) {
		throw std::logic_error("Pack length is not positive number");
		return 0;
	}

	return blockLen;
}

