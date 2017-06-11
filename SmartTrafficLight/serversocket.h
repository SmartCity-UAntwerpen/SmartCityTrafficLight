#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include "project.h"

#ifdef __cplusplus	//Check if the compiler is C++
	extern "C"	//Code needs to be handled as C-style code
	{
#endif

#define MESSAGE_SIZE 1024
#define RESPONSE_SIZE 256
#define MAX_CLIENTCONNECTIONS 2

/**
 * @brief callback type for the command PacketReceived
 */
typedef size_t PacketReceivedCallback_t(char* message, char* response, size_t maxLength);
typedef void* HandleConnectionCallback_t(void* args);

typedef enum socket_mode
{
    SOCKET_UDP,
    SOCKET_TCP,
    SOCKET_MAX
} socket_mode;

typedef struct connection_t
{
    bool active;
    bool abort;
    int connectionSocket;
    pthread_t connectionThread;
    pthread_mutex_t connectionBusy;
    PacketReceivedCallback_t* packetReceivedCallback;
    struct connection_t* next;
} connection_t;

typedef struct socket_t
{
    int serverSocket;
    socket_mode mode;
    int listeningPort;
    bool socketOpen;
    bool listening;
    pthread_t socketListenerThread;
    PacketReceivedCallback_t* packetReceivedCallback;
    HandleConnectionCallback_t* handleConnectionCallback;

    //struct connection_t* connections;
    int connections;    //Temporarily implementation (Only one connection allowed)
} socket_t;

int initialiseSocket(socket_t* socket_p, int port, socket_mode mode);
int releaseSocket(socket_t* socket_p);

int startListening(socket_t* socket_p);
int stopListening(socket_t* socket_p);

int closeConnections(socket_t* socket_p);

/**
 * @brief callback setter for the command PacketReceived
 * @param callback new callback for the command PacketReceived
 */
void setPacketReceivedCallback(socket_t* socket_p, PacketReceivedCallback_t callback);

void setConnectionHandleCallback(socket_t* socket_p, HandleConnectionCallback_t callback);

int socketReady(socket_t* socket_p);

void* listeningUDPThread(void* args);

void* listeningTCPThread(void* args);

void* handleTaskTCPConnection(void* args);

void* handleEventTCPConnection(void* args);

int getSocketPort(socket_t* socket_p);

ssize_t readLine(int connection_p, char* msg, size_t maxLength, bool* abortFlag);

ssize_t writeLine(int connection_p, const char* msg, size_t length);

#ifdef __cplusplus		//Check if the compiler is C++
	}		//End the extern "C" bracket
#endif

#endif
