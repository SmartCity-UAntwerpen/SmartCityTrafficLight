#ifndef SMARTCORE_H
#define SMARTCORE_H

#include <stdlib.h>
#include <stdbool.h>
#include "serversocket.h"
#include "robotapp.h"


    //class SmartCore
    //{
       // public:
            /*
             * Get singleton instance
             */
            //static SmartCore* getInstance();

            /*
             * Destructor
             */
            //virtual ~SmartCore(void);

            int initialiseCore(int argc, char *argv[]);

            int run(void);

            void stop(void);

            bool isRunning(void);

            size_t processCommand(char* command, char* response, size_t maxLength);

            size_t processDriveCommand(char* command, char* response, size_t maxLength);

            size_t processCameraCommand(char* command, char* response, size_t maxLength);

            size_t processTagCommand(char* command, char* response, size_t maxLength);

            size_t processLiftCommand(char* command, char* response, size_t maxLength);

            size_t processSpeakerCommand(char* command, char* response, size_t maxLength);

//        private:
            //static SmartCore* smartCore_instance;
            bool running;
            bool abort_;

            /*
             * Default constructor
             */
            SmartCore(void);

            /*
             * Copy constructor
             */
            //SmartCore(const SmartCore& core);

            /*
             * Assignment operator
             */
            //SmartCore& operator=(SmartCore const& core);

            int startProcesses(void);

            int stopProcesses(void);
  //  };

size_t receivedCommand(char* command, char* response, size_t maxLength);

#endif
