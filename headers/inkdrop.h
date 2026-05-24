// NOTE: [C Header files](inkdrop://note/UmdDKJ0X)
#ifndef INKDROP
#define INKDROP
#include "config.h"
#define UNUSED(x) (void)(x)
#define TAG_NAME_LEN 20
#define TAG_URL_LEN 20
#define NOTE_ID_LEN 20

#include <curl/curl.h>
typedef enum { None, Active, OnHold, Completed, Dropped } InkdropStatus;

typedef struct {
    char         *title;
    char         *url;
    InkdropStatus status;
    char         *description;
    char        **tags;
    int           ntags;
} InkdropNote;

typedef struct {
    char  *buffer;
    size_t size;
} ResponseBuffer;

/**
 * Query the local inkdrop http api to get the node based on it's id.
 * Filter the note to exclude completed and dropped notes unless all parameter is set to 1.
 */
InkdropNote inkdrop_get_note(char *url, CURL *curl, int all);

void inkdrop_handle_note_list(char *, CURL *curl, Paramters params);

/**
 * Free the note and all it's component
 */
void inkdrop_free_note(InkdropNote *note);

/**
 * Print the given note to the console
 */
void inkdrop_print_note(InkdropNote *note);

#endif
