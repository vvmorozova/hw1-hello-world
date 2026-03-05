#include "Writer.h"
#include <fstream>
#include <string>
#include <vector>

void Writer::writeResult(const std::string &filename, const std::vector<std::string> &result)
{
	std::ofstream out;
	out.open(filename);
	if (out.is_open()) {
		for (const auto & cmd: result) {
			out << cmd << ", ";
		}
		out.close();
	}
}
