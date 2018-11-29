#include "robotapp.h"
#include "stddef.h"
#include "serversocket.c"
#include <libpiface-1.0/pfio.h>


socket_t TCP_TaskSocket;

bool running;
bool abort_;

int initialiseDriver(int argc, char *argv[]);
size_t receivedCommand(char* command, char* response, size_t maxLength);
size_t processCommand(char* command, char* response, size_t maxLength);
size_t processLightCommand(char* command, char* response, size_t maxLength);
int run();
void stop();
int startSockets();
int stopSockets();
bool isRunning();


void RobotApp(int argc, char *argv[])
{
    pfio_init();

    running = false;
    abort_ = false;

    if(initialiseDriver(argc, argv) != 0)
    {
        printf("Driver initialisation failed!\n");
        printf("System will shutdown...\n");
        return;
    }

    printf("Driver ready...\n");

    run();

    pfio_deinit();
    printf ("Ready.\n");

        //Release server sockets
    releaseSocket(&TCP_TaskSocket);
}

int initialiseDriver(int argc, char *argv[])
{
    int res;

    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
    printf("Initiliasing TrafficLightDriver..\n");

    //Initialise configuration
    //Load default configuration
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);

    //Initialise serversockets
    //Task serversocket
    //AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init task serversocket...");
    res = initialiseSocket(&TCP_TaskSocket, LISTENINGPORT, SOCKET_TCP);
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
        printf("task socket initialised!\n");
    }

    //Set message received callback
    setPacketReceivedCallback(&TCP_TaskSocket, receivedCommand);
    setConnectionHandleCallback(&TCP_TaskSocket, handleTaskTCPConnection);


    return 0;
}

int run()
{

    int status = 0;

    running = true;

    status = startSockets();

    if(status > 0)
    {
        printf("Driver failed to start system! Error code: %d\n", status);
        //System failed to start processes
        running = false;
        abort_ = false;

        return status;
    }

    while(!abort_)
    {
        /*//Program loop
        pfio_digital_write(LIGHT1RED, OFF);
        pfio_digital_write(LIGHT1GREEN, ON);
        //pfio_digital_write(LIGHT2, RED);
        _delay_ms(SWITCH_TIME);
        pfio_digital_write(LIGHT1GREEN, OFF);
        pfio_digital_write(LIGHT1RED, ON);

        _delay_ms(CYCLE_TIME);
        _delay_ms(200);
        */

    }

    status = stopSockets();

    running = false;
    abort_ = false;

    return status;
}

void stop()
{
    if(running)
    {
        abort_ = true;
    }
}

int startSockets()
{
    //Start server sockets
    startListening(&TCP_TaskSocket);
    return 0;
}

int stopSockets()
{
    //Stop server sockets
    stopListening(&TCP_TaskSocket);

    return 0;
}

bool isRunning()
{
    return running;
}

size_t receivedCommand(char* command, char* response, size_t maxLength)
{
    return processCommand(command, response, maxLength);
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
        functionResponse = "KNOWN COMMANDS: LIGHT <1/2> <RED/GREEN/OFF>, HELP, SHUTDOWN";
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
        pfio_digital_write(LIGHT1RED, ON);
        pfio_digital_write(LIGHT1GREEN, OFF);
        functionResponse = "ACK";
        _delay_ms(1000);

    }
    else if(strcmp(command, "LIGHT 2 RED") == 0)
    {
        pfio_digital_write(LIGHT2RED, ON);
        pfio_digital_write(LIGHT2GREEN, OFF);
        functionResponse = "ACK";
        _delay_ms(1000);

    }
    else if(strcmp(command, "LIGHT 1 GREEN") == 0)
    {

        pfio_digital_write(LIGHT1GREEN, ON);
        pfio_digital_write(LIGHT1RED, OFF);
        functionResponse = "ACK";
        _delay_ms(1000);

    }
    else if(strcmp(command, "LIGHT 2 GREEN") == 0)
    {
        pfio_digital_write(LIGHT2GREEN, ON);
        pfio_digital_write(LIGHT2RED, OFF);
        functionResponse = "ACK";
        _delay_ms(1000);

    }
    else if(strcmp(command, "LIGHT 1 OFF") == 0)
    {
        pfio_digital_write(LIGHT1RED, OFF);
        pfio_digital_write(LIGHT1GREEN, OFF);
        functionResponse = "ACK";
        _delay_ms(1000);

    }
    else if(strcmp(command, "LIGHT 2 OFF") == 0)
    {
        pfio_digital_write(LIGHT2RED, OFF);
        pfio_digital_write(LIGHT2GREEN, OFF);
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



