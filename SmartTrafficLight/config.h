#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus	//Check if the compiler is C++
	extern "C"	//Code needs to be handled as C-style code
	{
#endif

typedef struct config_t
{
    char* key;
    char* value;
} config_t;

typedef enum ConfigKey
{
    CONFIG_TRAFFICLIGHTNAME,
    CONFIG_LISTENINGPORT,
    CONFIG_PUBLISHPORT,
    CONFIG_SERVERURL,
    CONFIG_LIFTACTIVE,
    CONFIG_MAX
} ConfigKey;

int initConfiguration(void);

int _initConfigPair(ConfigKey configKey, char* key, char* value);

int deinitConfiguration(void);

int setConfigValue(char* key, char* value);

int setConfigValueWithKey(ConfigKey key, char* value);

char* getConfigKeyName(ConfigKey key);

char* getConfigValue(ConfigKey key);

#ifdef __cplusplus	//Check if the compiler is C++
	}		//End the extern "C" bracket
#endif

#endif
