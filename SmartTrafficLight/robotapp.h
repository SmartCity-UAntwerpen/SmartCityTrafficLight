#ifndef ROBOT_APP
#define ROBOT_APP

#include "project.h"
#include "lin-delay.h"
#include "TimeSupport.h"
#include "config.h"
#include "configfile.h"
#include "serversocket.h"
#include <stdlib.h>


#define CYCLE_TIME 2000 //Red-green time (ms)
#define SWITCH_TIME 1000 //Time between red-green switch where all lights are red (ms)

#define LIGHT1 0
#define LIGHT2 1
#define GREEN 1
#define RED 0

void RobotApp(int argc, char *argv[]);

#endif
