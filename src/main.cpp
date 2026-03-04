#include <string>
#include <iostream>
#include <ctime>
#include <vector>
#include <fstream>
#include <chrono>

int main(int argc, char ** argv) {
	std::string input;
	int brackets = 0;
	int blockLen = 0;
	std::vector<std::string> cmds;
	std::time_t timeFirst;

	if (argc < 2) {
		std::cout << "No bulk length, exit" << std::endl;
		return 0;
	}

	blockLen = std::atoi(argv[1]);
	if (blockLen < 1) {
		std::cout << "Wrong bulk length, exit" << std::endl;
		return 0;
	}

	while (std::getline(std::cin, input)) {
		if (input == "{") {
			brackets++;
			if (brackets == 1) {
				// new time
				// print everything saved
			}
		}
		else if (input == "}") {
			brackets--;
		}
		else {
			if (cmds.size() == 0) {
				timeFirst = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				std::cout << "timeFirst =" << timeFirst << std::endl;
			}
			cmds.push_back(input);
			if (cmds.size() == (long unsigned int)blockLen) {
				std::string filename = "bulk" + std::to_string(timeFirst);
				filename += ".log";

				std::ofstream out;
				out.open(filename);
				if (out.is_open()) {
					for (const auto & cmd: cmds) {
						out << cmd << ", ";
					}
					out.close();
				}
				cmds.clear();
			}
		}
	}
	
	return 0;
} 


