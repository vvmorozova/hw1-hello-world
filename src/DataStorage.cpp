#include "DataStorage.h"

void DataStorage::addToPackCmd(std::string sCmd)
{
	m_packCmd.push_back(sCmd);
}

void DataStorage::clearPackCmd()
{
	m_packCmd.clear();
}

std::vector<std::string> &DataStorage::getPackCmd()
{
	return m_packCmd;
}

int DataStorage::getPackCmdSize()
{
	return m_packCmd.size();
}

void DataStorage::setFilename(std::string newName)
{
	m_filename = newName;
}

std::string DataStorage::getFilename()
{
	return m_filename;
}

void DataStorage::setBrackets(int brackets)
{
	m_brackets = brackets;
}

int DataStorage::getBrackets()
{
	return m_brackets;
}

void DataStorage::setBlockLen(int blockLen)
{
	m_blockLen = blockLen;
}

int DataStorage::getBlockLen()
{
	return m_blockLen;
}
