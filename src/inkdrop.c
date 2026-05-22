#include "../headers/inkdrop.h"

#include "../headers/config.h"

#include <cjson/cJSON.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t inkdrop_handle_tag_request(char *buffer, size_t itemsize, size_t nitems, void *_) {
    size_t bytes = itemsize * nitems;

    return bytes;
}

void display_tags(cJSON *note) {
    Config *config = get_config();
    cJSON *tags = cJSON_GetObjectItemCaseSensitive(note, "tags");
    cJSON *tag;

    // assuming that each tag:<tag_id> is no longer than 20 char long
    char  *tag_url   = malloc(sizeof(char) * strlen(config->inkdorp_url) + strlen("/note:XXXXXXXXXXXXXX"));
    CURL  *curl      = config->curl;

    printf("\tTags: ");
    // TODO: include a caching system for the tags, avoiding to endlessly querrying the API
    cJSON_ArrayForEach(tag, tags) {
        // Requesting each tags with inkdrop api to display it's name
        sprintf(tag_url, "%s/%s", config->inkdorp_url, tag->valuestring);
        curl_easy_setopt(curl, CURLOPT_URL, tag_url);
    }
    printf("\n");

    free(tag_url);
}

size_t inkdrop_handle_request(char *buffer, size_t itemsize, size_t nitems, void *_) {
    size_t bytes      = itemsize * nitems;
    cJSON *note       = cJSON_Parse(buffer);

    display_tags(note);

    // ---------- Displaying Status ----------
    cJSON *status = cJSON_GetObjectItemCaseSensitive(note, "status");
    printf("\tStatus : %s", status->valuestring);
    // ---------------------------------------

    // Cleaning the json tree, also clean all the subgenerated cJSONs
    cJSON_Delete(note);
    return bytes;
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
