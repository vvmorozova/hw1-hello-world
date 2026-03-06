#pragma once

#include <string>
#include <vector>

namespace Writer {
	void writeResultToFile(const std::string &filename, const std::vector<std::string> &result);
	void writeResultStream(std::ostream &out, const std::vector<std::string> &result);
};