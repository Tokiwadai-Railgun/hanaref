#include "../headers/inkdrop.h"

#include "../headers/config.h"

#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t inkdrop_handle_request(char *buffer, size_t itemsize, size_t nitems, void *custom_data);

size_t inkdrop_handle_tag_request(char *buffer, size_t itemsize, size_t nitems, void *ignored) {
    UNUSED(ignored);
    size_t bytes = itemsize * nitems;

    return bytes;
}

char **get_tags(cJSON *note) {
    Config *config = get_config();
    cJSON  *tags   = cJSON_GetObjectItemCaseSensitive(note, "tags");
    cJSON  *tag;

    size_t url_len  = sizeof(char) * strlen(config->inkdorp_url) + TAG_URL_LEN;
    char  *tag_url  = malloc(url_len);
    CURL  *curl     = config->curl;
    char **tags_arr = malloc(cJSON_GetArraySize(tags) * sizeof(char *));

    printf("\tTags: ");
    // TODO: include a caching system for the tags, avoiding to endlessly querrying the API for the same tags
    int i = 0;
    cJSON_ArrayForEach(tag, tags) {
        char *tag_name = malloc(TAG_NAME_LEN * sizeof(char));

        // Requesting each tags with inkdrop api to display it's name
        snprintf(tag_url, url_len, "%s/%s", config->inkdorp_url, tag->valuestring);
        curl_easy_setopt(curl, CURLOPT_URL, tag_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, inkdrop_handle_tag_request);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, tags_arr);
        curl_easy_perform(curl);

        // IMPORTANT: Reset the function for the following note request to proceed
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, inkdrop_handle_request);
        tags_arr[i] = tag_name;
        i++;
    }
    printf("\n");

    free(tag_url);

    return tags_arr;
}

size_t inkdrop_handle_request(char *buffer, size_t itemsize, size_t nitems, void *custom_data) {
    InkdropNote *return_note = custom_data;

    size_t bytes = itemsize * nitems;
    cJSON *note  = cJSON_Parse(buffer);
    cJSON *title = cJSON_GetObjectItemCaseSensitive(note, "title");

    printf("[%s]\n", title->valuestring);

    char **tags       = get_tags(note);
    return_note->tags = tags;

    // ---------- Displaying Status ----------
    cJSON *status = cJSON_GetObjectItemCaseSensitive(note, "status");
    printf("\tStatus : %s\n", status->valuestring);
    // ---------------------------------------

    // Cleaning the json tree, also clean all the subgenerated cJSONs
    cJSON_Delete(note);
    return bytes;
}

InkdropNote inkdrop_get_note(char *note_id, CURL *curl) {
    char       *inkdrop_url = getenv("INKDROP_URL");
    InkdropNote return_value;

    // Build the note url
    char *url = malloc(sizeof(char) * (strlen(inkdrop_url) + strlen(note_id) + 7));
    sprintf(url, "%s/note:%s", inkdrop_url, note_id);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &return_value);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_perform(curl);

    free(url);
    return return_value;
}

void inkdrop_handle_note_list(char *note_list, CURL *curl) {
    char *password = getenv("INKDROP_PASSWORD");
    char *username = getenv("INKDROP_USERNAME");
    curl_easy_setopt(curl, CURLOPT_USERNAME, username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, inkdrop_handle_request);

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

void inkdrop_free_note(InkdropNote *note) {
    // free the name
    free(note->name);
    free(note->description);

    // Free all the tags
    for (int i = 0; i < note->ntags; i++) {
        free(note->tags[i]);
    }
    free(note->tags);
}
