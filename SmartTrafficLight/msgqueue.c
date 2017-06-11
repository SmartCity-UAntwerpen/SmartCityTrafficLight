#include "msgqueue.h"

int startQueue(msgqueue_t* msgqueue)
{
    msgqueue->queueActiveFlag = true;

    if(!msgqueue->queueThreadRunning)
    {
        if(pthread_create(&msgqueue->queueThread, NULL, (void* (*)(void*))msgqueue->processMessage, NULL) < 0)
        {
            printf("Error while creating dispatch thread!\n");

            return 2;
        }

        msgqueue->queueThreadRunning = true;

        return 0;
    }
    else
    {
        return 1;
    }
}

int stopQueue(msgqueue_t* msgqueue)
{
    msgqueue->queueActiveFlag = false;

    //Wait for dispatch thread to finish
    pthread_join(msgqueue->queueThread, NULL);

    msgqueue->queueThreadRunning = false;

    return 0;
}

int flushQueue(msgqueue_t* msgqueue)
{
    msg_t *msgPointer;

    pthread_mutex_lock(&msgqueue->queueLock);

    while(msgqueue->queuePointer != NULL)
    {
        //Set temparory pointer to first element
        msgPointer = msgqueue->queuePointer;

        //Switch front of queue to next element
        msgqueue->queuePointer = msgqueue->queuePointer->Next;

        //Free msgPointer
        freeMsg(msgPointer);
    }

    pthread_mutex_unlock(&msgqueue->queueLock);

    return 0;
}

int addMsg(msgqueue_t* msgqueue, msg_t* message)
{
    msg_t *msgPointer;

    pthread_mutex_lock(&msgqueue->queueLock);

    //Check if queue is empty
    if(msgqueue->queuePointer == NULL)
    {
        msgqueue->queuePointer = message;
    }
    else
    {
        msgPointer = msgqueue->queuePointer;

        while(msgPointer->Next != NULL)
        {
            msgPointer = msgPointer->Next;
        }

        //Add new message to the end of the queue
        msgPointer->Next = message;
    }

    pthread_mutex_unlock(&msgqueue->queueLock);

    return 0;
}

bool queueIsEmpty(msgqueue_t* msgqueue)
{
    if(msgqueue == NULL)
    {
        //Message queue can not be null
        return true;
    }

    if(msgqueue->queuePointer == NULL)
    {
        //Queue empty
        return true;
    }
    else
    {
        //Message available
        return false;
    }
}

msg_t* getNextMsg(msgqueue_t* msgqueue)
{
    struct msg_t* msgPointer = NULL;

    if(msgqueue == NULL)
    {
        //Message queue can not be null
        return NULL;
    }

    pthread_mutex_lock(&msgqueue->queueLock);

    if(msgqueue->queuePointer == NULL)
    {
        //Message queue is empty
        pthread_mutex_unlock(&msgqueue->queueLock);

        return NULL;
    }

    msgPointer = msgqueue->queuePointer;

    //Remove message from queue
    msgqueue->queuePointer = msgqueue->queuePointer->Next;

    pthread_mutex_unlock(&msgqueue->queueLock);

    return msgPointer;
}

int freeMsg(msg_t* message)
{
    if(message == NULL)
    {
        //No message to free
        return 1;
    }

    if(message->values != NULL)
    {
        //Free message values
        free(message->values);
    }

    //Free message
    free(message);

    return 0;
}
