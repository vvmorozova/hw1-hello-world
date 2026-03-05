#include "InputHandler.h"
#include "Writer.h"

#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>

void InputHandler::handle(std::string input)
{
	if (input == "{") {
		m_pData->incBrackets();

		if (m_pData->getBrackets() == 1) {

			handleWriteRes();
			generateFilename();
		}
	}
	else if (input == "}") {
		m_pData->decBrackets();

		if (m_pData->getBrackets() == 0) {

			handleWriteRes();
			generateFilename();
		}
	}
	else {
		if (m_pData->getPackCmdSize() == 0) {
			generateFilename();
		}

		m_pData->addToPackCmd(input);

		if (m_pData->getPackCmdSize() == (int)m_pData->getBlockLen() && m_pData->getBrackets() == 0) {
			handleWriteRes();
		}
	}
}

void InputHandler::generateFilename()
{
	std::time_t  timeFirst = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	)
	.count();

	m_pData->setFilename("bulk" + std::to_string(timeFirst) + ".log");

}

void InputHandler::handleWriteRes()
{
	Writer::writeResult(m_pData->getFilename(), m_pData->getPackCmd());
	m_pData->clearPackCmd();
}
