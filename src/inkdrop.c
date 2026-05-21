#include "../headers/inkdrop.h"

#include "../headers/config.h"

#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t inkdrop_handle_request(char *buffer, size_t itemsize, size_t nitems, void *_) {
    size_t bytes      = itemsize * nitems;
    int    linenumber = 1;
    printf("New chunk (%zu bytes)\n", bytes);
    printf("%d:\t%s", linenumber, buffer);
    cJSON *response = cJSON_Parse(buffer);

    cJSON *tags = cJSON_GetObjectItemCaseSensitive(response, "tags");
    printf("\tTags : ");
    cJSON *tag;

    char **tags_id = malloc(cJSON_GetArraySize(tags) * sizeof(char) *
                            20); // assuming that each tag:<tag_id> is no longer than 20 char long
    cJSON_ArrayForEach(tag, tags) {
        printf("%s, ", tag->valuestring);
    }

    printf("\n");

    // Cleaning the json tree, also clean all the subgenerated cJSONs
    cJSON_Delete(response);
    free(tags_id);
    return bytes;
}

char *inkdrop_get_tag(char *tag_id) {
    Config *conf = get_config();
    CURL   *curl = conf->curl;
}

InkdropNote inkdrop_get_note(char *note_id, CURL *curl) {
    char *inkdrop_url = getenv("INKDROP_URL");

    // Build the note url
    char *url = malloc(sizeof(char) * (strlen(inkdrop_url) + strlen(note_id) + 6));
    sprintf(url, "%s/note:%s", inkdrop_url, note_id);

    printf("Url is : %s\n", url);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_perform(curl);

    InkdropNote return_value;

    free(url);
    return return_value;
}

void inkdrop_handle_note_list(char *note_list, CURL *curl) {
    char *password = getenv("INKDROP_PASSWORD");
    char *username = getenv("INKDROP_USERNAME");
    curl_easy_setopt(curl, CURLOPT_USERNAME, username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, inkdrop_handle_request);

    char  buffer[256];
    char *line = strtok(note_list, "\n");
    while (line != NULL) {
        do {
            line++;
        } while (line[0] != '/');
        // Remove one more last char
        line += 7;
        line[strlen(line) - 1] = '\0'; // remove the closing parenthesis
        inkdrop_get_note(line, curl);

        line = strtok(NULL, "\n");
    }
}
