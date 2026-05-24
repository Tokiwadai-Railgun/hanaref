#include "../headers/inkdrop.h"

#include "../headers/config.h"
#include "../headers/terminal.h"

#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t inkdrop_handle_request(char *buffer, size_t itemsize, size_t nitems, void *custom_data);
void   inkdrop_print_note(InkdropNote *note);

InkdropStatus str_to_status(char *status_name) {
    if (strcmp(status_name, "active") == 0) {
        return Active;
    } else if (strcmp(status_name, "onHold") == 0) {
        return OnHold;
    } else if (strcmp(status_name, "completed") == 0) {
        return Completed;
    } else if (strcmp(status_name, "dropped") == 0) {
        return Dropped;
    }

    return None;
}

char *status_to_str(InkdropStatus status) {
    switch (status) {
    case None:
        return "";
    case Active:
        return BLUE("(Active)");
    case OnHold:
        return ORANGE("(On Hold)");
    case Completed:
        return GREEN("(Completed)");
    case Dropped:
        return RED("(Dropped)");
    }

    return "";
}

size_t inkdrop_handle_tag_request(char *buffer, size_t itemsize, size_t nitems, void *custom_data) {
    // here we do not need to aggregate buffers, because tags request will never be long enough to require 2 chunks of
    // data
    size_t bytes    = itemsize * nitems;
    char  *tag_name = custom_data;

    // extract tag infos
    cJSON *tag_info = cJSON_Parse(buffer);
    cJSON *name     = cJSON_GetObjectItemCaseSensitive(tag_info, "name");
    strcpy(tag_name, cJSON_GetStringValue(name));

    return bytes;
}

void extract_tags(cJSON *note, InkdropNote *return_value) {
    Config *config = get_config();
    cJSON  *tags   = cJSON_GetObjectItemCaseSensitive(note, "tags");
    cJSON  *tag;

    size_t url_len  = sizeof(char) * strlen(config->inkdorp_url) + TAG_URL_LEN;
    char  *tag_url  = malloc(url_len);
    CURL  *curl     = config->curl;
    char **tags_arr = malloc(cJSON_GetArraySize(tags) * sizeof(char *));

    // TODO: include a caching system for the tags, avoiding to endlessly
    // querrying the API for the same tags
    int i = 0;
    cJSON_ArrayForEach(tag, tags) {
        char *tag_name = malloc(TAG_NAME_LEN * sizeof(char));

        // Requesting each tags with inkdrop api to display it's name
        snprintf(tag_url, url_len, "%s/%s", config->inkdorp_url, tag->valuestring);
        curl_easy_setopt(curl, CURLOPT_URL, tag_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, inkdrop_handle_tag_request);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, tag_name);
        curl_easy_perform(curl);

        // IMPORTANT: Reset the function for the following note request to
        // proceed
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, inkdrop_handle_request);
        tags_arr[i] = tag_name;
        i++;
    }
    return_value->ntags = cJSON_GetArraySize(tags);
    return_value->tags  = tags_arr;

    free(tag_url);
}

size_t inkdrop_handle_request(char *buffer, size_t itemsize, size_t nitems, void *custom_data) {
    // [libcurl handlers](inkdrop://note/S27DZDxL)
    // which means that custom data now contains a buffer being resized and completed for each chunk of data
    size_t          bytes           = itemsize * nitems;
    ResponseBuffer *response_buffer = custom_data;

    response_buffer->buffer = realloc(response_buffer->buffer, bytes + response_buffer->size + 1);
    if (response_buffer == NULL) {
        // probably out of memory
        printf("Unable to resize response buffer : %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    memcpy(response_buffer->buffer + response_buffer->size, buffer, bytes);
    response_buffer->size += bytes;
    response_buffer->buffer[response_buffer->size] = '\0';

    return bytes;
}

InkdropNote inkdrop_get_note(char *note_id, CURL *curl) {
    char       *inkdrop_url = getenv("INKDROP_URL");
    InkdropNote return_value;

    // Build the note url
    // [snprintf coredump coredump](inkdrop://note/GK5iLwSy)
    size_t url_len = sizeof(char) * (strlen(inkdrop_url) + strlen(note_id) + 7);
    char  *url     = malloc(url_len);
    snprintf(url, url_len, "%s/note:%s", inkdrop_url, note_id);

    char *local_url = malloc((strlen("inkdrop://note/") + NOTE_ID_LEN) * sizeof(char));
    sprintf(local_url, "inkdrop://note/%s", note_id);
    return_value.url = local_url;

    // ----- Get the note -----
    ResponseBuffer response_buffer;
    response_buffer.size   = 0;
    response_buffer.buffer = malloc(sizeof(char));

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_perform(curl);

    cJSON *note   = cJSON_Parse(response_buffer.buffer);
    cJSON *title  = cJSON_GetObjectItemCaseSensitive(note, "title");
    cJSON *status = cJSON_GetObjectItemCaseSensitive(note, "status");

    char *title_value = malloc((strlen(cJSON_GetStringValue(title)) + 1) * sizeof(char));
    strcpy(title_value, title->valuestring);
    return_value.title  = title_value;
    return_value.status = str_to_status(status->valuestring);

    // extract the tags from the cJSON and insert them into the InkdropNote struct
    extract_tags(note, &return_value);

    // Cleaning the json tree, also clean all the subgenerated cJSONs
    cJSON_Delete(note);
    // ----- Get tags for note -----

    free(url);
    inkdrop_print_note(&return_value);
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
    free(note->title);
    free(note->description);

    // Free all the tags
    for (int i = 0; i < note->ntags; i++) {
        free(note->tags[i]);
    }
    free(note->tags);
}

void inkdrop_print_note(InkdropNote *note) {
    printf("%s [%s] \x1b[38;5;25m\e]8;;%s\e\\open\e]8;;\e\\\x1b[39m\n", status_to_str(note->status), note->title, note->url);

    printf("\tTags -> ");
    // Print tags
    for (int i = 0; i < note->ntags - 1; i++) {
        printf("%s, ", note->tags[i]);
    }
    printf("%s", note->tags[note->ntags - 1]);
    printf("\n");
}
