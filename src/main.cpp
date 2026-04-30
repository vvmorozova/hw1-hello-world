#include <string>
#include <iostream>
#include "cmd_args_parser.h"

int main(int argc, char ** argv) {
	cmd_args_parser parser;
    try {
        config cfg = parser.parse(argc, argv);
    } catch (const cmd_args_parser::help_req& h) {
        std::cout << h.what() << "\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
	return 0;
} 


