#include "config.h"

static config_t configuration[CONFIG_MAX];

int initConfiguration(void)
{
    //Initialise default configuration
    //Config Carname
    if(_initConfigPair(CONFIG_TRAFFICLIGHTNAME, "trafficLightName", "TrafficLight") > 0)
    {
        printf("Could not allocate memory for configuration!\n");

        return 1;
    }

    //Config Task Socket Port
    if(_initConfigPair(CONFIG_LISTENINGPORT, "listeningport", "1315") > 0)
    {
        printf("Could not allocate memory for configuration!\n");

        return 1;
    }

    //Config Event Socket Port
    if(_initConfigPair(CONFIG_PUBLISHPORT, "publishport", "1316") > 0)
    {
        printf("Could not allocate memory for configuration!\n");

        return 1;
    }

    //Config Server URL
    if(_initConfigPair(CONFIG_SERVERURL, "serverurl", "") > 0)
    {
        printf("Could not allocate memory for configuration!\n");

        return 1;
    }
    return 0;
}

int _initConfigPair(ConfigKey configKey, char* key, char* value)
{
    configuration[configKey].key = (char*) malloc (strlen(key) + 1);
    configuration[configKey].value = (char*) malloc (strlen(value) + 1);

    if(configuration[configKey].key == NULL || configuration[configKey].value == NULL)
    {
        return 1;
    }

    strcpy(configuration[configKey].key, key);
    strcpy(configuration[configKey].value, value);

    return 0;
}

int deinitConfiguration(void)
{
    int i;

    for(i = 0; i < CONFIG_MAX; i++)
    {
        if(configuration[i].key != NULL)
        {
            free(configuration[i].key);

            configuration[i].key = NULL;
        }

        if(configuration[i].value != NULL)
        {
            free(configuration[i].value);

            configuration[i].value = NULL;
        }
    }

    return 0;
}

int setConfigValue(char* key, char* value)
{
    int i = 0;
    bool found = false;

    while(!found && i < CONFIG_MAX)
    {
        if(strcmp(configuration[i].key, key) == 0)
        {
            found = true;
        }
        else
        {
            i++;
        }
    }

    if(!found)
    {
        return 2;
    }

    return setConfigValueWithKey((ConfigKey) i, value);
}

int setConfigValueWithKey(ConfigKey key, char* value)
{
    if(key >= CONFIG_MAX)
    {
        //Invalid configuration key
        return 2;
    }

    //Deallocate previous value
    if(configuration[key].value != NULL)
    {
        free(configuration[key].value);
    }

    configuration[key].value = (char*) malloc (strlen(value) + 1);

    if(configuration[key].value == NULL)
    {
        printf("Could not allocate memory for setting configuration value of key: %d!\n", key);

        return 1;
    }

    memcpy(configuration[key].value, value, strlen(value));
    configuration[key].value[strlen(value)] = '\0';

    return 0;
}

char* getConfigKeyName(ConfigKey key)
{
    if(key >= CONFIG_MAX)
    {
        //Invalid configuration key
        return NULL;
    }

    return configuration[key].key;
}

char* getConfigValue(ConfigKey key)
{
    if(key >= CONFIG_MAX)
    {
        //Invalid configuration key
        return NULL;
    }

    return configuration[key].value;
}
