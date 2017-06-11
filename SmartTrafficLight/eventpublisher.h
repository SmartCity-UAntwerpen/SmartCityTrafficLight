#ifndef EVENTPUBLISHER_H
#define EVENTPUBLISHER_H

#include <string.h>
#include "msgqueue.h"

#ifdef __cplusplus	//Check if the compiler is C++
	extern "C"	//Code needs to be handled as C-style code
	{
#endif

int initEventPublisher(void);

int deinitEventPublisher(void);

int startEventPublisher(void);

int stopEventPublisher(void);

int publishEvent(msg_t* event);

bool eventAvailable(void);

msg_t* getNextEvent(void);

char* getNextEventString(void);

int flushAllEvents(void);

#ifdef __cplusplus		//Check if the compiler is C++
	}		//End the extern "C" bracket
#endif

#endif
