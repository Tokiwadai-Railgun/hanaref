#include "../headers/config.h"
#include "../headers/inkdrop.h"

#include <curl/curl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int check_for_env() {
    struct stat buffer;
    int         dot_env = open(".env", O_RDONLY);
    fstat(dot_env, &buffer);

    printf("Size is : %ld", buffer.st_size);
    return 0;
}

void help() {
    printf("hanaref - v0.0.1\n");
    printf("extract Notion & Inkdrop notes from the codebase to a complete list of information\n");
    printf("Options are : \n\n");
    printf("--tag <tag>\n\tfilter response and return only notes containing the given tag (Inkdrop Only)\n");
    printf("--bug\n\tFilter response to only bug tasks / notes (bug tag for Inkdrop, Bug category for Notion)\n");
    printf("--all\n\tGive all the notes (incluing completed and dropped ones)\n");
}

/*
 * Arguments 1 will contains the list of Inkdrop Notes
 */
int main(int argc, char **args) {
    // Extract arguments
    if (strcmp(args[1], "--help") == 0) {
        help();
        return EXIT_SUCCESS;
    }

    Paramters params;
    params.bug = 0;
    params.all = 0;
    params.ntags = 0;
    params.tags = malloc(sizeof(char *));
    for (int i = 2; i < argc; i++) {
        if (strcmp(args[i], "--help") == 0) {
            help();
            return EXIT_SUCCESS;
        } else if (strcmp(args[i], "--bug") == 0) {
            params.bug = 1;
        } else if (strcmp(args[i], "--all") == 0) {
            params.all = 1;
        } else if (strcmp(args[i], "--tag") == 0) {
            if (argc <= i + 1) {
                printf("Wrong usage : --tag <tag>\n");
                return EXIT_FAILURE;
            }

            params.ntags += 1;
            char *tag = malloc(strlen(args[i + 1]) * sizeof(char));
            memcpy(tag, args[i+1], strlen(args[i+1]) * sizeof(char));
            params.tags = realloc(params.tags, params.ntags);
            params.tags[params.ntags - 1] = tag;
        }
    }

    load_dotenv(); // Initialise dotenv
    CURL   *curl = curl_easy_init();
    Config *conf = init_config();
    conf->curl   = curl;

    inkdrop_handle_note_list(args[1], curl, params);

    // Not required to clean the params memory, in fact it avoids a loop
    curl_easy_cleanup(curl);
    return EXIT_SUCCESS;
}
