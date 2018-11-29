#include "serversocket.h"
#define DEBUG_SOCKET

int initialiseSocket(socket_t* socket_p, int port, socket_mode mode)
{
	int length;
	int flags;
	int optValue;
	struct sockaddr_in server;

	if(socket_p == NULL)
	{
        //Socket can not be NULL
        return 1;
    }

    if(socket_p->socketOpen)
    {
        //Socket needs first to be closed
        return 2;
    }

    if(mode == SOCKET_UDP)
    {
        //Setup UDP socket
        socket_p->serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    }
    else if(mode == SOCKET_TCP)
    {
        //Setup TCP socket
        socket_p->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        //Allow to recover port after restart
        optValue = 1;   //Set value True
        setsockopt(socket_p->serverSocket, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof optValue);
    }
    else
    {
        printf("Socket mode: %d is no valid input!\n", mode);

        return 3;
    }

    //Set non-blocking flag
    flags = fcntl(socket_p->serverSocket, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(socket_p->serverSocket, F_SETFL, flags);

    if(socket_p->serverSocket < 0)
    {
        printf("Error while initiliasing socket on port: %d\n", port);

        close(socket_p->serverSocket);

        return 4;
    }

	length = sizeof(server);
	memset(&server, 0, length);	//Initialise socket address server

	server.sin_family = AF_INET;			//Address family
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);	        //Set port address

	//Bind socket address
	if(bind(socket_p->serverSocket, (struct sockaddr*)&server, length) < 0)
	{
		printf("Error while binding socket on port: %d\n", port);

		close(socket_p->serverSocket);

		return 5;
	}

	socket_p->listeningPort = port;
	socket_p->socketOpen = true;
	socket_p->mode = mode;
	socket_p->listening = false;
	socket_p->handleConnectionCallback = NULL;
	//socket_p->connections = NULL; //Temporarily implementation (ONLY one connection allowed)
    socket_p->connections = 0;

	return 0;
}

int releaseSocket(socket_t* socket_p)
{
	if(socket_p->listening)
	{
		printf("Socket is still listening on port %d. Stop listening first before trying to release it.\n", socket_p->listeningPort);

		return 1;
	}

	if(close(socket_p->serverSocket) < 0)
	{
		printf("Error while closing socket.\n");

		return 2;
	}

	socket_p->socketOpen = false;

    #ifdef DEBUG_SOCKET
	printf("Socket on port: %d is successfully closed.\n", socket_p->listeningPort);
    #endif

	return 0;
}

int startListening(socket_t* socket_p)
{
	int result = 0;

	if(!socket_p->listening && socket_p->socketOpen)
	{
		if(socket_p->mode == SOCKET_UDP)
		{
            //UDP listening thread
            result = pthread_create(&(socket_p->socketListenerThread), NULL, listeningUDPThread, socket_p);
        }
        else if(socket_p->mode == SOCKET_TCP)
        {
            //TCP listening thread
            result = pthread_create(&(socket_p->socketListenerThread), NULL, listeningTCPThread, socket_p);
        }
        else
        {
            //Unknown socket mode
            return 3;
        }

		if(result < 0)
		{
			printf("Error while creating listening thread for port: %d!\n", socket_p->listeningPort);

			return 2;
		}

		socket_p->listening = true;
	}
	else
	{
		return 1;
	}

	return 0;
}

int stopListening(socket_t* socket_p)
{
	if(socket_p->listening)
	{
		socket_p->listening = false;

		//Wait till thread is finished
		pthread_join(socket_p->socketListenerThread, NULL);

		if(closeConnections(socket_p) > 1)
		{
            //Error while closing connections
            return 2;
		}

		return 0;
	}

	return 1;
}

int closeConnections(socket_t* socket_p)
{
    /**
    **  TEMPORARILY IMPLEMENTATION: Only one connection allowed
    **/
/*  connection_t* connectionPointer;

    if(socket_p->connections == NULL)
    {
        //No connections to close
        return 1;
    }

    //Close all connections
    while(socket_p->connections != NULL)
    {
        //Get first connection
        connectionPointer = socket_p->connections;

        //Set connections pointer to next connection
        socket_p->connections = socket_p->connections->next;

        //Abort connection
        connectionPointer->abort = true;

        //Wait for connection to close
        pthread_join(connectionPointer->connectionThread, NULL);

        //Free memory
        free(connectionPointer);
    }
*/
    return 0;
}

void setPacketReceivedCallback(socket_t* socket_p, PacketReceivedCallback_t callback)
{
	socket_p->packetReceivedCallback = callback;
}

void setConnectionHandleCallback(socket_t* socket_p, HandleConnectionCallback_t callback)
{
    socket_p->handleConnectionCallback = callback;
}

int socketReady(socket_t* socket_p)
{
	return socket_p->listening;
}

void* listeningUDPThread(void* args)
{
	int dataLength;
	char buffer[MESSAGE_SIZE];
	socklen_t clientLength;
	struct sockaddr_in client;

	clientLength = sizeof(struct sockaddr_in);

    #ifdef DEBUG_SOCKET
	printf("Listening on port: %d (UDP)\n", ((socket_t*)args)->listeningPort);
    #endif

	while(((socket_t*)args)->listening)
	{
		memset(&buffer, 0, MESSAGE_SIZE);

		dataLength = recvfrom(((socket_t*)args)->serverSocket, buffer, MESSAGE_SIZE, 0, (struct sockaddr*)&client, &clientLength);

		if(dataLength >= 0)
		{
		    //handle the task
			((socket_t*)args)->packetReceivedCallback(buffer, NULL, 0);
		}
		else
		{
            //No message, sleep
            _delay_ms(100);
		}
	}

	return NULL;
}

void* listeningTCPThread(void* args)
{
    int socketConnection;
    int flags;
    socket_t* socket = (socket_t*)args;

    #ifdef DEBUG_SOCKET
	printf("Listening on port: %d (TCP)\n", socket->listeningPort);
    #endif

    listen(socket->serverSocket, MAX_CLIENTCONNECTIONS);

    while(socket->listening)
    {
        socketConnection = accept(socket->serverSocket, NULL, NULL);

        if(socketConnection >= 0)
        {
            //Only allows one connection at the time
            //TO DO: Allow multiple connections

            //Set non-blocking flag
            flags = fcntl(socketConnection, F_GETFL);
            flags |= O_NONBLOCK;
            fcntl(socketConnection, F_SETFL, flags);

            #ifdef DEBUG_SOCKET
            printf("Client connected on port: %d (TCP)\n", socket->listeningPort);
            #endif

            //Add connection to TCP socket
            socket->connections = socketConnection;

            if(socket->handleConnectionCallback != NULL)
            {
                socket->handleConnectionCallback(socket);
            }

            #ifdef DEBUG_SOCKET
            printf("Client disconnected on port: %d (TCP)\n", socket->listeningPort);
            #endif

            //Close connection
            close(socketConnection);
        }
        else
        {
            //No connection, sleep
            _delay_ms(100);
        }
    }

    return NULL;
}
void* handleTaskTCPConnection(void* args)
{
    int readStatus;
    size_t responseSize;
    char response[RESPONSE_SIZE] = "\0";
	char buffer[MESSAGE_SIZE] = "\0";

    socket_t* serverSocket = (socket_t*) args;
    int socketConnection = serverSocket->connections;

    //Send greeting message
    strcpy(response, "TrafficLightDriver - Version: 0.0.4 \r\n#\0");

	writeLine(socketConnection, response, strlen(response));

    while(serverSocket->listening)
    {
        readStatus = readLine(socketConnection, buffer, MESSAGE_SIZE, &(serverSocket->listening));

        if(readStatus > 0)
        {
            //Remove newline characters
            strtok(buffer, "\r\n");

            responseSize = serverSocket->packetReceivedCallback(buffer, response, RESPONSE_SIZE);

            #ifdef DEBUG_SOCKET
            printf("Message: %s - Response: %s\n", buffer, response);
            #endif

            //Send callback response
            writeLine(socketConnection, response, responseSize);

            strcpy(response, "\r\n# ");
            writeLine(socketConnection, response, strlen(response));
        }
        else if(readStatus < 0)
        {
            //Error occured while reading socket
            #ifdef DEBUG_SOCKET
            printf("Error reading line from socket on port: %d (TCP)\n", serverSocket->listeningPort);
            #endif

            return NULL;
        }
        else if(readStatus == 0)
        {
            //Connection closed
            return NULL;
        }
    }

	return NULL;
}


int getSocketPort(socket_t* socket_p)
{
    return socket_p->listeningPort;
}

ssize_t readLine(int connection_p, char* msg, size_t maxLength, bool* listeningFlag)
{
    ssize_t n, result;
    char c;
    bool newLine = false;

    n = 0;
    while(n < maxLength && !newLine && *listeningFlag)
    {
        result = read(connection_p, &c, 1);

        if(result == 1)
        {
            *(msg + n) = c;
            n++;

            if(c == '\n')
            {
               newLine = true;
            }
        }
        else if(result == 0)
        {
            if(n == 1)
            {
                return 0;
            }
            else
            {
                newLine = true;
            }
        }
        else if(result == -1)
        {
            //No character to read
            _delay_ms(100);
        }
        else
        {
            if(errno != EINTR)
            {
                return -1;
            }
        }
    }

    if(*listeningFlag)
    {
        *(msg + n) = '\0';
        return n;
    }
    else
    {
        //Reading interrupted
        return -1;
    }
}

ssize_t writeLine(int connection_p, const char* msg, size_t length)
{
    size_t nLeft;
    ssize_t nWritten;
    const char* buffer;

    buffer = msg;
    nLeft = length;

    while(nLeft > 0)
    {
        if((nWritten = send(connection_p, buffer, nLeft, MSG_NOSIGNAL)) <= 0)
        {
            if(errno == EINTR)
            {
                nWritten = 0;
            }
            else
            {
                return -1;
            }
        }

        nLeft -= nWritten;
        buffer += nWritten;
    }

    return length;
}
