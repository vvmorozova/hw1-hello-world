#pragma once

#include <vector>
#include <string>

class DataStorage {
	std::vector<std::string> m_packCmd;
	std::string m_filename;
	int m_brackets;
	int m_blockLen;

public:
	DataStorage() : m_brackets(0), m_blockLen(0) {};

	void addToPackCmd(std::string sCmd);
	void clearPackCmd();
	std::vector<std::string> &getPackCmd();
	int getPackCmdSize();

	void setFilename(std::string);
	std::string getFilename();

	void setBrackets(int);
	int getBrackets();

	void setBlockLen(int);
	int getBlockLen();

};
