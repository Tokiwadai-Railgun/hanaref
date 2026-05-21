// NOTE: [C Header files](inkdrop://note/UmdDKJ0X)
#ifndef CONFIG
#define CONFIG

#include <curl/curl.h>

typedef struct {
    CURL *curl;
} Config;
/*
 * Read the .env file and add the environment variables to path
 */
void env_init();

Config* get_config();

#endif
