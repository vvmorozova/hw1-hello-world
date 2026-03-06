#include "Writer.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

void Writer::writeResultToFile(const std::string &filename, const std::vector<std::string> &result)
{
	std::ofstream out;
	out.open(filename);
	if (out.is_open()) {
		writeResultStream(out, result);
		out.close();
	}
}

void Writer::writeResultStream(std::ostream &out, const std::vector<std::string> &result)
{
	int size = result.size();
	if (out.rdbuf() == (std::cout).rdbuf()) {
		std::cout << "bulk: ";
	}
	for (int i = 0; i < size; i++) {
		std::string sEnd = i == (size - 1) ? "\n" : ", ";
		out << result[i] << sEnd;
	}
}
