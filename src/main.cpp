#include <string>
#include <iostream>
#include <fstream>
#include <memory>

#include "InputHandler.h"
#include "Reader.h"
#include "InitArgsHandler.h"
#include "DataStorage.h"

int main(int argc, char ** argv) {
	std::string input;

	std::shared_ptr<DataStorage> pdataStorage = std::make_shared<DataStorage>();

	try {

		InitArgsHandler initArgsHandler(argc, argv);
		initArgsHandler.checkArgsNumber();
		pdataStorage->setBlockLen(initArgsHandler.getBlockLen());
	}
	catch (std::logic_error &e) {
		std::cout << e.what() << std::endl;
		return 0;
	}

	InputHandler inputHandler(pdataStorage);
	while (Reader::readLine(std::cin, input)) {
		inputHandler.handle(input);
	}
	
	return 0;
} 


