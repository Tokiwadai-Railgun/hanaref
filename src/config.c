#include "../headers/config.h"

#include <curl/curl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config conf;

void load_dotenv() {
    char buffer[256];

    //[C String parsing](inkdrop://note/TKe409Fc)
    FILE *dotenv = fopen(".env", "r");
    if (dotenv == NULL) {
        fprintf(stderr, "Unable to open .env file : %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), dotenv)) {
        char *name  = strtok(buffer, "=");
        char *value = strtok(NULL, "=");
        // remove eventual first and last quotes

        if (value[0] == '"')
            value++;
        if (value[strlen(value) - 2] == '"')
            value[strlen(value) - 2] = '\0';

        setenv(name, value, 1);
    }
}

/**
 * Load environment variables to the config var  
 *
 * WARNING, only load environment variables, curl remains undefined at this step
 */
Config* init_config() {
  conf.inkdorp_url = getenv("INKDROP_URL");
  return &conf;
}

Config* get_config() {
  return &conf;
}
