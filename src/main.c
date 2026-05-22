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
    load_dotenv(); // Initialise dotenv
    CURL   *curl = curl_easy_init();
    Config *conf = init_config();
    conf->curl   = curl;

    inkdrop_handle_note_list(args[1], curl);

    curl_easy_cleanup(curl);
    return EXIT_SUCCESS;
}
