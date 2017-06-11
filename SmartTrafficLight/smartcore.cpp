#include "smartcore.h"
#include <string>

using namespace SC;
using namespace cv;

//Single instance of class
SmartCore* SmartCore::smartCore_instance = NULL;

//Default constructor
SmartCore::SmartCore()
{
    this->running = false;
    this->abort = false;
}

SmartCore::~SmartCore()
{
    stopProcesses();

    //Stop REST Interface
    stopRestInterface();

    //Release server sockets
    releaseSocket(&TCP_TaskSocket);
    releaseSocket(&TCP_EventSocket);

    //Stop camera
    closeCamera();

    //Destroy configuration
    deinitConfiguration();
}

//Get singleton instance
SmartCore* SmartCore::getInstance()
{
    if(!smartCore_instance)
    {
        smartCore_instance = new SmartCore();
    }

    return smartCore_instance;
}

int SmartCore::initialiseCore(int argc, char *argv[])
{
    int res;

    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
    printf("Initialising SmartCore...\n");

    //Initialise configuration
    //Load default configuration
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Load configuration...");
    res = initConfiguration();
    if(res > 0)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initConfiguration() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 1;
    }

    //Read configuration if available
    res = readConfigFile("sc-conf");
    if(res > 1)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: loadConfigFile() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 2;
    }
    else if(res == 1)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_YELLOW);
        printf("WARNING: no configuration file found. loadConfigFile() error code %d\n", res);
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Initialise drivequeue
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init drivequeue...");
    res = initDriveQueue();
    if(res > 0)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initDriveQueue() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 3;
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Set event callback
    setDriveFinishedCallback(driveFinishedEvent);

    //Setup motordrivers
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init motordrivers...");
    res = DriveInit();
    if(res > 0)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: DriveInit() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
  //      return 4;
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Initialise event publisher
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init event publisher...");
    res = initEventPublisher();
    if(res > 0)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initEventPublisher() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 5;
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Initialise process modules
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init process modules...");
    res = initProcessModules();
    if(res > 1)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initProcessModules() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 6;
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Intialise restinterface
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init REST-controller...");
    res = initRestInterface();
    if(res > 1)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initRestInterface() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 7;
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Initialise serversockets
    //Task serversocket
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init task serversocket...");
    res = initialiseSocket(&TCP_TaskSocket, atoi(getConfigValue(CONFIG_LISTENINGPORT)), SOCKET_TCP);
    if(res > 1)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initialiseSocket() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 8;
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Set message received callback
    setPacketReceivedCallback(&TCP_TaskSocket, receivedCommand);
    setConnectionHandleCallback(&TCP_TaskSocket, handleTaskTCPConnection);

    //Event serversocket
    AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
    printf("Init event serversocket...");
    res = initialiseSocket(&TCP_EventSocket, atoi(getConfigValue(CONFIG_PUBLISHPORT)), SOCKET_TCP);
    if(res > 1)
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_RED);
        printf("FAIL: initialiseSocket() error code %d\n", res);

        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_WHITE);
        return 9;
    }
    else
    {
        AnsiSetColor(ANSI_ATTR_OFF,ANSI_BLACK,ANSI_GREEN);
        printf("OK\n");
    }

    //Set message received callback
    setPacketReceivedCallback(&TCP_EventSocket, receivedCommand);
    setConnectionHandleCallback(&TCP_EventSocket, handleEventTCPConnection);

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
    return 0;
}

int SmartCore::run()
{

    int count = 0;

    /*while(count < 4){
        Mat im;
        VideoCapture cap = getCameraCapture();
        cap.read(im);

        if(count ==0){
            imwrite("test0.jpg", im);
        }
        if(count ==1){
            imwrite("test1.jpg", im);
        }
        if(count ==2){
            imwrite("test2.jpg", im);
        }
        if(count ==3){
            imwrite("test3.jpg", im);
        }
        if(count ==4){
            imwrite("test4.jpg", im);
        }

        count++;
        _delay_ms(2000);
    }*/

    int status = 0;

    this->running = true;

    status = startProcesses();

    if(status > 0)
    {
        printf("Core failed to start system! Error code: %d\n", status);

        //System failed to start processes
        this->running = false;
        this->abort = false;

        return status;
    }

    while(!this->abort)
    {
        //Program loop
        _delay_ms(200);
    }

    status = stopProcesses();

    this->running = false;
    this->abort = false;

    return -status;
}

void SmartCore::stop()
{
    if(this->running)
    {
        this->abort = true;
    }
}

int SmartCore::startProcesses()
{
    if(startQueue(getDriveQueue()) > 0)
    {
        //Could not start drive queue
        return 1;
    }

    //Start watchdog
    //startWatchdog();

    //Start server sockets
    startListening(&TCP_TaskSocket);
    startListening(&TCP_EventSocket);

    return 0;
}

int SmartCore::stopProcesses()
{
    //Stop drive queue
    if(getDriveQueue() != NULL)
    {
        stopQueue(getDriveQueue());

        deinitDriveQueue();
    }

    //Stop watchdog
    stopWatchdog();

    //Stop driving
    AbortDriving();

    //Stop process modules
    stopProcessModules();

    //Stop server sockets
    stopListening(&TCP_TaskSocket);
    stopListening(&TCP_EventSocket);

    return 0;
}

bool SmartCore::isRunning()
{
    return this->running;
}

size_t SmartCore::processCommand(char* command, char* response, size_t maxLength)
{
    char const* functionResponse;

    if(strncmp(command, "DRIVE", 5) == 0)
    {
        //Drive commands
        return processDriveCommand(command, response, maxLength);
    }
    else if(strncmp(command, "CAMERA", 6) == 0)
    {
        //Camera commands
        return processCameraCommand(command, response, maxLength);
    }
    else if(strncmp(command, "TAG", 3) == 0)
    {
        //Tag commands
        return processTagCommand(command, response, maxLength);
    }
    else if(strncmp(command, "LIFT", 4) == 0)
    {
        //Lift commands
        return processLiftCommand(command, response, maxLength);
    }
    else if(strncmp(command, "SPEAKER", 7) == 0)
    {
        //Speaker commands
        return processSpeakerCommand(command, response, maxLength);
    }
    else if(strcmp(command, "SHUTDOWN") == 0)
    {
        //Shutdown command
        this->stop();

        functionResponse = "ACK";
    }
    else if(strcmp(command, "HELP") == 0 || strcmp(command, "?") == 0)
    {
        //Help command
        functionResponse = "KNOWN COMMANDS: DRIVE [ABORT, FLUSH, FOLLOWLINE, PAUSE, RESUME, FORWARD, BACKWARDS, TURN, ROTATE, DISTANCE], CAMERA [TRAFFICLIGHT], TAG [READ UID], LIFT [GOTO, HEIGHT], SPEAKER [MUTE, UNMUTE, PLAY, SAY, STOP], SHUTDOWN, HELP";
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

size_t SmartCore::processDriveCommand(char* command, char* response, size_t maxLength)
{
    msg_t* message;
    char const* functionResponse;

    if(strcmp(command, "DRIVE ABORT") == 0)
    {
        flushQueue(getDriveQueue());
        AbortDriving();

        functionResponse = "ACK";
    }
    else if(strcmp(command, "DRIVE FLUSH") == 0)
    {
        flushQueue(getDriveQueue());

        functionResponse = "ACK";
    }
    else if(strncmp(command, "DRIVE FOLLOWLINE", 16) == 0)
    {
        int distValue = 0;
        int* msgValues;

        message = (struct msg_t*) malloc (sizeof(struct msg_t));

        message->Next = NULL;

        if(strlen(command) > 17)
        {
            //Follow line for given distance
            message->id = DRIVE_FOLLOWLINE_DISTANCE;
            message->numOfParm = 1;

            msgValues = (int*) malloc (sizeof(int));

            distValue = atoi(&command[17]);

            memcpy(msgValues, &distValue, sizeof(int));
            message->values = msgValues;
        }
        else
        {
            //Follow line until end of line
            message->id = DRIVE_FOLLOWLINE;
            message->numOfParm = 0;
            message->values = NULL;
        }

        addMsg(getDriveQueue(), message);

        functionResponse = "ACK";
    }
    else if(strcmp(command, "DRIVE PAUSE") == 0)
    {
        pauseDriving();

        functionResponse = "ACK";
    }
    else if(strcmp(command, "DRIVE RESUME") == 0)
    {
        continueDriving();

        functionResponse = "ACK";
    }
    else if(strncmp(command, "DRIVE FORWARD", 13) == 0)
    {
        int distanceValue = 0;
        int* msgValues;

        if(strlen(command) > 14)
        {
            message = (struct msg_t*) malloc (sizeof(struct msg_t));
            msgValues = (int*) malloc (sizeof(int));

            message->id = DRIVE_STRAIGHT_DISTANCE;
            message->numOfParm = 1;
            message->Next = NULL;

            distanceValue = atoi(&command[14]);

            memcpy(msgValues, &distanceValue, sizeof(int));
            message->values = msgValues;

            addMsg(getDriveQueue(), message);

            functionResponse = "ACK";
        }
        else
        {
            functionResponse = "MALFORMED COMMAND";
        }
    }
    else if(strncmp(command, "DRIVE BACKWARDS", 15) == 0)
    {
        int distanceValue = 0;
        int* msgValues;

        if(strlen(command) > 16)
        {
            message = (struct msg_t*) malloc (sizeof(struct msg_t));
            msgValues = (int*) malloc (sizeof(int));

            message->id = DRIVE_BACKWARDS_DISTANCE;
            message->numOfParm = 1;
            message->Next = NULL;

            distanceValue = atoi(&command[16]);

            memcpy(msgValues, &distanceValue, sizeof(int));
            message->values = msgValues;

            addMsg(getDriveQueue(), message);

            functionResponse = "ACK";
        }
        else
        {
            functionResponse = "MALFORMED COMMAND";
        }
    }
    else if(strncmp(command, "DRIVE TURN", 10) == 0)
    {
        msg_id_drive direction;
        int* msgValues;

        if(command[11] == 'L')
        {
            //Turn left
            direction = DRIVE_ANGLE_LEFT;
        }
        else if(command[11] == 'R')
        {
            //Turn right
            direction = DRIVE_ANGLE_RIGHT;
        }
        else
        {
            //Unknown command
            direction = MSG_ID_DRIVE_TOTAL;
            functionResponse = "MALFORMED COMMAND";
        }

        if(direction < MSG_ID_DRIVE_TOTAL)
        {
            int angleValue = 0;
            message = (struct msg_t*) malloc (sizeof(struct msg_t));
            msgValues = (int*) malloc (sizeof(int));

            message->id = direction;
            message->numOfParm = 1;
            message->Next = NULL;

            if(strlen(command) > 13)
            {
                angleValue = atoi(&command[13]);
            }
            else
            {
                angleValue = 90;
            }

            memcpy(msgValues, &angleValue, sizeof(int));
            message->values = msgValues;

            addMsg(getDriveQueue(), message);

            functionResponse = "ACK";
        }
    }
    else if(strncmp(command, "DRIVE ROTATE", 12) == 0)
    {
        msg_id_drive direction;
        int* msgValues;

        if(command[13] == 'L')
        {
            //Rotate left
            direction = DRIVE_ROTATE_LEFT;
        }
        else if(command[13] == 'R')
        {
            //Rotate right
            direction = DRIVE_ROTATE_RIGHT;
        }
        else
        {
            //Unknown command
            direction = MSG_ID_DRIVE_TOTAL;
            functionResponse = "MALFORMED COMMAND";
        }

        if(direction < MSG_ID_DRIVE_TOTAL && strlen(command) > 15)
        {
            int angleValue = 0;
            message = (struct msg_t*) malloc (sizeof(struct msg_t));
            msgValues = (int*) malloc (sizeof(int));

            message->id = direction;
            message->numOfParm = 1;
            message->Next = NULL;

            angleValue = atoi(&command[15]);

            memcpy(msgValues, &angleValue, sizeof(int));
            message->values = msgValues;

            addMsg(getDriveQueue(), message);

            functionResponse = "ACK";
        }
    }
    else if(strncmp(command, "DRIVE DISTANCE", 14) == 0)
    {
        getDriveDistance(NULL);

        functionResponse = "ACK";
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

size_t SmartCore::processCameraCommand(char* command, char* response, size_t maxLength)
{
    char const* functionResponse;

    if(strcmp(command, "CAMERA TRAFFICLIGHT") == 0)
    {
        if(startTrafficLightDetection() == 0)
        {
            functionResponse = "ACK";
        }
        else
        {
            functionResponse = "NACK";
        }
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

size_t SmartCore::processTagCommand(char* command, char* response, size_t maxLength)
{
    char const* functionResponse;

    if(strcmp(command, "TAG READ UID") == 0)
    {
        if(startReadTagUID() == 0)
        {
            functionResponse = "ACK";
        }
        else
        {
            functionResponse = "NACK";
        }
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

size_t SmartCore::processLiftCommand(char* command, char* response, size_t maxLength)
{
    char const* functionResponse;

    if(strncmp(command, "LIFT GOTO", 9) == 0)
    {
        if(startLiftGoto(atoi(&command[10])) == 0)
        {
            functionResponse = "ACK";
        }
        else
        {
            functionResponse = "NACK";
        }
    }
    else if(strcmp(command, "LIFT HEIGHT") == 0)
    {
        if(liftInitialised())
        {
            getLiftHeight(NULL);

            functionResponse = "ACK";
        }
        else
        {
            functionResponse = "NACK";
        }
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

size_t SmartCore::processSpeakerCommand(char* command, char* response, size_t maxLength)
{
    char const* functionResponse;

    if(strcmp(command, "SPEAKER MUTE") == 0)
    {
        setSpeakerMute(true);

        functionResponse = "ACK";
    }
    else if(strcmp(command, "SPEAKER UNMUTE") == 0)
    {
        setSpeakerMute(false);

        functionResponse = "ACK";
    }
    else if(strncmp(command, "SPEAKER PLAY", 12) == 0)
    {
        playWav(&command[13]);

        functionResponse = "ACK";
    }
    else if(strncmp(command, "SPEAKER SAY", 11) == 0)
    {
        espeak(&command[12]);

        functionResponse = "ACK";
    }
    else if(strcmp(command, "SPEAKER STOP") == 0)
    {
        stopSpeaker();

        functionResponse = "ACK";
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

//Wrapper function
size_t receivedCommand(char* command, char* response, size_t maxLength)
{
    SmartCore* core = SmartCore::getInstance();

    return core->processCommand(command, response, maxLength);
}
