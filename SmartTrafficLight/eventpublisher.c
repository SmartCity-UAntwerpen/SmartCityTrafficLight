#include "eventpublisher.h"
#include "msgqueue.c"

static pthread_mutex_t queueLock;

static struct msgqueue_t* eventQueue;

int initEventPublisher(void)
{
    //Allocate message queue struct
    eventQueue = (msgqueue_t*) malloc (sizeof(msgqueue_t));

    if(eventQueue != NULL)
    {
        //Initialise parameters
        eventQueue->processMessage = NULL;
        eventQueue->queueActiveFlag = false;
        eventQueue->queueLock = queueLock;
        eventQueue->queuePointer = NULL;
        eventQueue->queueThread = 0;
        eventQueue->queueThreadRunning = false;

        return 0;
    }
    else
    {
        //Queue is not properly allocated!
        return 1;
    }
}

int deinitEventPublisher(void)
{
    if(eventQueue == NULL)
    {
        //Event queue is not initialised
        return 1;
    }

    if(eventQueue->queueActiveFlag == true)
    {
        //Can not destroy event queue while active
        return 2;
    }

    //Flush all queue elements
    flushQueue(eventQueue);

    //Free memory of drive queue
    free(eventQueue);

    eventQueue = NULL;

    return 0;
}

int startEventPublisher(void)
{
    if(eventQueue == NULL)
    {
        //Eventqueue is not initialised yet
        return 1;
    }

    eventQueue->queueActiveFlag = true;

    return 0;
}

int stopEventPublisher(void)
{
    if(eventQueue == NULL)
    {
        //Eventqueue is not initialised
        return 1;
    }

    eventQueue->queueActiveFlag = false;

    return 0;
}

int publishEvent(msg_t* event)
{
    if(eventQueue == NULL)
    {
        //Eventqueue is not initialised
        return 1;
    }

    if(eventQueue->queueActiveFlag)
    {
        if(event->type == EVENT_MSG)
        {
            return addMsg(eventQueue, event);
            return 0;
        }
        else
        {
            //Message has wrong type
            return 3;
        }
    }
    else
    {
        //Event publisher not active, drop message
        return 2;
    }
}

bool eventAvailable(void)
{
    return !queueIsEmpty(eventQueue);
}

msg_t* getNextEvent(void)
{
    return getNextMsg(eventQueue);
}

char* getNextEventString(void)
{
    msg_t* event;
    char* eventString;
    char eventMessage[1024] = "\0";
    char buffer[128];

    event = getNextEvent();

    if(event == NULL)
    {
        //No event available
        return NULL;
    }

    switch(event->id)
    {
        case EVENT_DRIVE_FINISHED:
            strcat(eventMessage, "DRIVE EVENT: FINISHED");
            break;
        case EVENT_TRAFFICLIGHT_DETECTION:
            strcat(eventMessage, "TRAFFICLIGHT DETECTION EVENT: ");
            strcat(eventMessage, (char*)event->values);
            break;
        case EVENT_TAG_DETECTION:
            strcat(eventMessage, "TAG DETECTION EVENT: ");
            strcat(eventMessage, (char*)event->values);
            break;
        case EVENT_TRAVEL_DISTANCE:
            sprintf(buffer, "%d", *((int*)event->values));
            strcat(eventMessage, "TRAVEL DISTANCE EVENT: ");
            strcat(eventMessage, buffer);
            break;
        case EVENT_LIFT_GOTO:
            strcat(eventMessage, "LIFT GOTO EVENT: ");
            strcat(eventMessage, (char*)event->values);
            break;
        case EVENT_LIFT_HEIGHT:
            sprintf(buffer, "%.0f", *((float*)event->values));
            strcat(eventMessage, "LIFT HEIGHT EVENT: ");
            strcat(eventMessage, buffer);
            break;
        default:
            //Invalid event identifier
            return NULL;
    }

    //Add end characters
    strcat(eventMessage, "\r\n");

    //Allocate event message string
    eventString = (char*) malloc (strlen(eventMessage) + 1);
    strcpy(eventString, eventMessage);

    //Free event message
    freeMsg(event);

    return eventString;
}

int flushAllEvents(void)
{
    return flushQueue(eventQueue);
}
