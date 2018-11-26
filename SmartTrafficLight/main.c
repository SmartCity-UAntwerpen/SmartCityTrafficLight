#include "../../minimod/Libs/lin-delay.h"
#include "../../minimod/Libs/lin-gpio.h"
#include "../../minimod/Libs/lego-motor.h"
#include "../../minimod/Libs/lego-sensor.h"
#include "../../minimod/Libs/rf-cc1101.h"
#include "../../minimod/Libs/datatypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include "robotapp.h"
#include "ansi.h"

//#define DEBUG_ABORT

pthread_t MasterThread;

void LogCsvSint16(char *FileName,sint16 *Data,uint16 Num);

void printBanner();

int main(int argc, char *argv[])
{
    //Force printf to print text to terminal
    setvbuf(stdout, NULL, _IONBF, 0);
    AnsiCls();
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
    printBanner();


    //Main application code
    RobotApp(argc,argv);

    return 0;
}

void printBanner()
{
    printf("%s%s%s%s%s%s%s%s%s",
           "   _____                      _    _____ _ _            \n",
           "  / ____|                    | |  / ____(_) |           \n",
           " | (___  _ __ ___   __ _ _ __| |_| |     _| |_ _   _    \n",
           "  \\___ \\| '_ ` _ \\ / _` | '__| __| |    | | __| | | |   \n",
           "  ____) | | | | | | (_| | |  | |_| |____| | |_| |_| |   \n",
           " |_____/|_| |_| |_|\\__,_|_|   \\__|\\_____|_|\\__|\\__, |   \n",
           "                                                __/ |   \n",
           " ==============================================|___/=   \n",
           "  :: SmartCity Traffic-Light - 2018 ::     (v. 0.0.1)   \n\n");
}
