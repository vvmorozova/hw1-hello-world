#pragma once

#ifndef LIB_DBG
#define LIB_DBG
	#define DEBUG_PRINT(msg) do {if (lib_debug) std::cout << msg << std::endl;} while (0);
	extern bool lib_debug ;
#endif

#include "allocator.h"
#include "container.h"


