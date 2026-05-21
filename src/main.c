#include "../headers/config.h"
#include "../headers/inkdrop.h"

#include <curl/curl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int check_for_env() {
    struct stat buffer;
    int         dot_env = open(".env", O_RDONLY);
    fstat(dot_env, &buffer);

    printf("Size is : %ld", buffer.st_size);
    return 0;
}

/*
 * Arguments 1 will contains the list of Inkdrop Notes
 */
int main(int argc, char **args) {
    CURL    *curl   = curl_easy_init();
    CURLcode result = curl_easy_perform(curl);

    Config* conf = get_config();
    conf->curl = curl;

    env_init(); // Initialise dotenv
    printf("Number of arguments : %d\n", argc);
    inkdrop_handle_note_list(args[1], curl);

    curl_easy_cleanup(curl);
    return EXIT_SUCCESS;
}

get_curl
