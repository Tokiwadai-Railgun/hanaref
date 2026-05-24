// NOTE: [C Header files](inkdrop://note/UmdDKJ0X)
#ifndef INKDROP
#define INKDROP
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

InkdropNote inkdrop_get_note(char *url, CURL *curl);

void inkdrop_handle_note_list(char *, CURL *curl);

/**
 * Free the note and all it's component
 */
void inkdrop_free_note(InkdropNote *note);

/**
 * Print the given note to the console
 */
void inkdrop_print_note(InkdropNote *note);

#endif
