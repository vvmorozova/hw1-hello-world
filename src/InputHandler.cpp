#include "InputHandler.h"
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>

void InputHandler::handle(std::string input)
{
	if (input == "{") {
			int brackets = m_pData->getBrackets();
			brackets++;
			m_pData->setBrackets(brackets);
			if (brackets == 1) {
				// new time
				// print everything saved
			}
		}
		else if (input == "}") {
			int brackets = m_pData->getBrackets();
			brackets--;
			m_pData->setBrackets(brackets);
		}
		else {
			std::time_t timeFirst;
			if (m_pData->getPackCmdSize() == 0) {
				timeFirst = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				m_pData->setFilename("bulk" + std::to_string(timeFirst) + ".log");
			}
			m_pData->addToPackCmd(input);
			if (m_pData->getPackCmdSize() == (int)m_pData->getBlockLen()) {
				std::ofstream out;
				out.open(m_pData->getFilename());
				if (out.is_open()) {
					for (const auto & cmd: m_pData->getPackCmd()) {
						out << cmd << ", ";
					}
					out.close();
				}
				m_pData->clearPackCmd();
			}
		}
}
