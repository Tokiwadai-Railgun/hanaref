// NOTE: [C Header files](inkdrop://note/UmdDKJ0X)
#ifndef INKDROP
#define INKDROP

#include <curl/curl.h>
typedef enum { Active, OnHold, Completed, Dropped } InkdropStatus;

typedef struct {
    char*         name;
    InkdropStatus status;
    char*         description;
    char**        tags;
} InkdropNote;

InkdropNote inkdrop_get_note(char* url, CURL* curl);

void inkdrop_handle_note_list(char*, CURL* curl);

#endif
