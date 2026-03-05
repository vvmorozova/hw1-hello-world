#include "Reader.h"

#include <string>
#include <iostream>
#include <fstream>

bool Reader::readLine(std::istream &input, std::string &line)
{
	return (bool)std::getline(input, line);
}