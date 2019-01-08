#ifndef ROBOT_APP
#define ROBOT_APP

#include "lin-delay.h"
#include "TimeSupport.h"
#include "serversocket.h"
#include <stdlib.h>


#define CYCLE_TIME 2000 //Red-green time (ms)
#define SWITCH_TIME 1000 //Time between red-green switch where all lights are red (ms)

#define LISTENINGPORT 1315
#define LIGHT1GREEN 0
#define LIGHT2GREEN 2
#define LIGHT1RED 1
#define LIGHT2RED 3
#define ON 1
#define OFF 0

void RobotApp(int argc, char *argv[]);

#endif
