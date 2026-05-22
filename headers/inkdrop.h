// NOTE: [C Header files](inkdrop://note/UmdDKJ0X)
#ifndef INKDROP
#define INKDROP
#define UNUSED(x) (void)(x)
#define TAG_NAME_LEN 20
#define TAG_URL_LEN 20

#include <curl/curl.h>
typedef enum { Active, OnHold, Completed, Dropped } InkdropStatus;

typedef struct {
    char         *name;
    InkdropStatus status;
    char         *description;
    char        **tags;
    int           ntags;
} InkdropNote;

InkdropNote inkdrop_get_note(char *url, CURL *curl);

void inkdrop_handle_note_list(char *, CURL *curl);

/**
 * Free the note and all it's component
 */
void inkdrop_free_note(InkdropNote *note);

#endif
