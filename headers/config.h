#ifndef CONFIG
#define CONFIG

#include <curl/curl.h>

typedef struct {
    char  curl_error[256];
    CURL *curl;
    char *inkdorp_url;
} Config;
/*
 * Read the .env file and add the environment variables to path
 */
void load_dotenv();

Config *init_config();

Config *get_config();

typedef struct {
    char **tags;
    int    ntags;
    int    bug;
    int    all;
} Paramters;
#endif
