#include "robotapp.h"
#include "stddef.h"
#include "config.c"
#include "configfile.c"
#include "serversocket.c"
#include <libpiface-1.0/pfio.h>


socket_t TCP_TaskSocket;
socket_t TCP_EventSocket;

bool running;
bool abort_;

int initialiseCore(int argc, char *argv[]);
size_t receivedCommand(char* command, char* response, size_t maxLength);
size_t processCommand(char* command, char* response, size_t maxLength);
size_t processLightCommand(char* command, char* response, size_t maxLength);
int run();
void stop();
int startProcesses();
int stopProcesses();
bool isRunning();


void RobotApp(int argc, char *argv[])
{
    pfio_init();

    running = false;
    abort_ = false;

    if(initialiseCore(argc, argv) != 0)
    {
        printf("Core initialisation failed!\n");
        printf("System will shutdown...\n");
        return;
    }

    printf("Core ready...\n");

    run();

    pfio_deinit();
    printf ("Ready.\n");

        //Release server sockets
    releaseSocket(&TCP_TaskSocket);
    releaseSocket(&TCP_EventSocket);

    //Destroy configuration
    deinitConfiguration();
}

int initialiseCore(int argc, char *argv[])
{
    int res;

    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
    printf("Initialising SmartCore...\n");

    //Initialise configuration
    //Load default configuration
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Load configuration...");
    res = initConfiguration();
    if(res > 0)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initConfiguration() error code %d\n", res);

        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 1;
    }

    //Read configuration if available
    res = readConfigFile("sc-conf");
    if(res > 1)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: loadConfigFile() error code %d\n", res);

        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 2;
    }
    else if(res == 1)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_YELLOW);
        printf("WARNING: no configuration file found. loadConfigFile() error code %d\n", res);
    }
    else
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Initialise drivequeue
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    //printf("Init drivequeue...");
    //res = initDriveQueue();
    /*if(res > 0)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initDriveQueue() error code %d\n", res);

        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 3;
    }
    else
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }*/

    //Set event callback
    //setDriveFinishedCallback(driveFinishedEvent);

    //Setup motordrivers
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    /*printf("Init motordrivers...");
    res = DriveInit();
    if(res > 0)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: DriveInit() error code %d\n", res);

        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    //      return 4;
    }
    else
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }*/

    //Initialise event publisher
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init event publisher...");
    res = initEventPublisher();
    if(res > 0)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initEventPublisher() error code %d\n", res);

        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 5;
    }
    else
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Initialise process modules
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    /*printf("Init process modules...");
    res = initProcessModules();
    if(res > 1)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initProcessModules() error code %d\n", res);

        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 6;
    }
    else
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }*/

    //Intialise restinterface
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    /*printf("Init REST-controller...");
    res = initRestInterface();
    if(res > 1)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initRestInterface() error code %d\n", res);

        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 7;
    }
    else
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }*/

    //Initialise serversockets
    //Task serversocket
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init task serversocket...");
    res = initialiseSocket(&TCP_TaskSocket, atoi(getConfigValue(CONFIG_LISTENINGPORT)), SOCKET_TCP);
    if(res > 1)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initialiseSocket() error code %d\n", res);

        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 8;
    }
    else
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Set message received callback
    setPacketReceivedCallback(&TCP_TaskSocket, receivedCommand);
    setConnectionHandleCallback(&TCP_TaskSocket, handleTaskTCPConnection);

    //Event serversocket
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init event serversocket...");
    res = initialiseSocket(&TCP_EventSocket, atoi(getConfigValue(CONFIG_PUBLISHPORT)), SOCKET_TCP);
    if(res > 1)
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initialiseSocket() error code %d\n", res);

        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 9;
    }
    else
    {
        //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Set message received callback
    setPacketReceivedCallback(&TCP_EventSocket, receivedCommand);
    setConnectionHandleCallback(&TCP_EventSocket, handleEventTCPConnection);

    /*
    //Initialise camera
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init camera...");
    res = initCamera();
    if(initCamera() > 1)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initCamera() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    //      return 10;
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Initialise map
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Read mapfile...");
    res = parseMapFile("MAP.dmap");
    if(res > 0)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_YELLOW);
        printf("WARNING: could not load map file. parseMapFile() error code %d\n", res);
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Initialise lift motor
    char* liftActive = getConfigValue(CONFIG_LIFTACTIVE);

    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Initialise lift...");

    if(strncmp(liftActive, "on", 2) == 0)
    {
        res = LiftInit();
        if(res > 0)
        {
            AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_YELLOW);
            printf("WARNING: could not initialise lift. LiftInit() error code %d\n", res);
        }
        else
        {
            AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
            printf("OK\n");
        }
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_CYAN);
        printf("DISABLED IN CONFIG\n");
    }

    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    */
    return 0;
}

int run()
{

    int status = 0;

    running = true;

    status = startProcesses();

    if(status > 0)
    {
        printf("Core failed to start system! Error code: %d\n", status);

        //System failed to start processes
        running = false;
        abort_ = false;

        return status;
    }

    while(!abort_)
    {
        const char* response = "TEST";
        writeLine(TCP_EventSocket.connections, response, strlen(response));
        //Program loop
        /*pfio_digital_write(LIGHT1, RED);
        pfio_digital_write(LIGHT2, RED);
        _delay_ms(SWITCH_TIME);
        pfio_digital_write(LIGHT1, RED);
        pfio_digital_write(LIGHT2, GREEN);
        _delay_ms(CYCLE_TIME);
        pfio_digital_write(LIGHT1, RED);
        pfio_digital_write(LIGHT2, RED);
        _delay_ms(SWITCH_TIME);
        pfio_digital_write(LIGHT1, GREEN);
        pfio_digital_write(LIGHT2, RED);
        _delay_ms(CYCLE_TIME);
        _delay_ms(200);*/

    }

    status = stopProcesses();

    running = false;
    abort_ = false;

    return -status;
}

void stop()
{
    if(running)
    {
        abort_ = true;
    }
}

int startProcesses()
{
    /*if(startQueue(getDriveQueue()) > 0)
    {
        //Could not start drive queue
        return 1;
    }*/

    //Start watchdog
    //startWatchdog();

    //Start server sockets
    startListening(&TCP_TaskSocket);
    startListening(&TCP_EventSocket);

    return 0;
}

int stopProcesses()
{
    //Stop drive queue
    /*if(getDriveQueue() != NULL)
    {
        stopQueue(getDriveQueue());

        deinitDriveQueue();
    }*/

    //Stop watchdog
    //stopWatchdog();

    //Stop driving
    //AbortDriving();

    //Stop process modules
    //stopProcessModules();

    //Stop server sockets
    stopListening(&TCP_TaskSocket);
    stopListening(&TCP_EventSocket);

    return 0;
}

bool isRunning()
{
    return running;
}

size_t receivedCommand(char* command, char* response, size_t maxLength)
{
    //SmartCore* core = SmartCore::getInstance();

    return processCommand(command, response, maxLength);//core->processCommand(command, response, maxLength);
}

size_t processCommand(char* command, char* response, size_t maxLength)
{
    char const* functionResponse;

    if(strncmp(command, "LIGHT", 5) == 0)
    {
        //Light commands
        return processLightCommand(command, response, maxLength);
    }
    else if(strcmp(command, "SHUTDOWN") == 0)
    {
        //Shutdown command
        stop();
        functionResponse = "ACK";
    }
    else if(strcmp(command, "HELP") == 0 || strcmp(command, "?") == 0)
    {
        //Help command
        functionResponse = "KNOWN COMMANDS: LIGHT 1/2 RED/GREEN, HELP, SHUTDOWN";
    }
    else
    {
        functionResponse = "UNKNOWN COMMAND";
    }

    if(response != NULL)
    {
        if(strlen(functionResponse) + 1 >= maxLength)
        {
            strncpy(response, functionResponse, maxLength - 2);
            response[maxLength - 1] = '\0';
        }
        else
        {
            strcpy(response, functionResponse);
            response[strlen(functionResponse)] = '\0';
        }

        return strlen(response);
    }
    else
    {
        return 0;
    }
}

size_t processLightCommand(char* command, char* response, size_t maxLength)
{
    char const* functionResponse;

    if(strcmp(command, "LIGHT 1 RED") == 0)
    {
        pfio_digital_write(LIGHT1, RED);
        functionResponse = "ACK";
        _delay_ms(1000);

    }
    else if(strcmp(command, "LIGHT 2 RED") == 0)
    {
        pfio_digital_write(LIGHT2, RED);
        functionResponse = "ACK";
        _delay_ms(1000);

    }
    else if(strcmp(command, "LIGHT 1 GREEN") == 0)
    {

        pfio_digital_write(LIGHT1, GREEN);
        functionResponse = "ACK";
        _delay_ms(1000);

    }
    else if(strcmp(command, "LIGHT 2 GREEN") == 0)
    {
        pfio_digital_write(LIGHT2, GREEN);
        functionResponse = "ACK";
        _delay_ms(1000);

    }
    else
    {
        functionResponse = "UNKNOWN COMMAND";
    }

    if(response != NULL)
    {
        if(strlen(functionResponse) + 1 >= maxLength)
        {
            strncpy(response, functionResponse, maxLength - 2);
            response[maxLength - 1] = '\0';
        }
        else
        {
            strcpy(response, functionResponse);
            response[strlen(functionResponse)] = '\0';
        }

        return strlen(response);
    }
    else
    {
        return 0;
    }
}



