#include "configfile.h"

int readConfigFile(char const* fileName)
{
    FILE* configFile;
    char filePath[128] = "./";
    char line[512];
    char* keyPointer;
    char* valuePointer;

    //File located in program work directory
    strcat(filePath, fileName);

    //Read configfile
    configFile = fopen(filePath, "r");

    if(configFile == NULL)
    {
        printf("Could not open file: %s\n", fileName);

        return 1;   //Could not read file
    }

    //Read each line of the config file
    while(!feof(configFile))
    {
        fgets(line, sizeof(line), configFile);

        //Ignore comment line
        if(line[0] != '#')
        {
            keyPointer = strtok(line, "=");

            //Check if line is not empty
            if(keyPointer != NULL)
            {
                valuePointer = strtok(NULL, "\r\n");

                //Check if value is not empty
                if(valuePointer != NULL)
                {
                    //Load configuration
                    setConfigValue(keyPointer, valuePointer);
                }
            }
        }
    }

    //Close configFile
    fclose(configFile);

    return 0;
}
