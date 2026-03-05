#pragma once

class InitArgsHandler {
	int m_argc;
	char **m_argv;

public:
	void checkArgsNumber();
	int getBlockLen();
	InitArgsHandler() = delete;
	InitArgsHandler(int argc, char **argv) : m_argc(argc), m_argv(argv) {};
};
