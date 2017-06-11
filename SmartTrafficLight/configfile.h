#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string.h>
#include "config.h"

#ifdef __cplusplus	//Check if the compiler is C++
	extern "C"	//Code needs to be handled as C-style code
	{
#endif

int readConfigFile(char const* fileName);

#ifdef __cplusplus	//Check if the compiler is C++
	}		//End the extern "C" bracket
#endif

#endif
