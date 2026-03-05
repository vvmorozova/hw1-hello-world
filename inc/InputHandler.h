#pragma once

#include "DataStorage.h"
#include <string>
#include <memory>

class InputHandler {
	std::shared_ptr<DataStorage> m_pData;
public:
	InputHandler() = delete;
	InputHandler(std::shared_ptr<DataStorage> pData) : m_pData(pData) {};
	void handle(std::string input);
};