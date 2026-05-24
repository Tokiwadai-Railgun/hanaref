#ifndef TERMINAL
#define TERMINAL

#define ORANGE(text) "\x1b[38;5;172m" text "\x1b[39m"
#define RED(text) "\x1b[38;5;160m" text "\x1b[39m"
#define BLUE(text) "\x1b[38;5;25m" text "\x1b[39m"
#define GREEN(text) "\x1b[38;5;22m" text "\x1b[39m"
#define PURPLE(text) "\x1b[38;5;171m" text "\x1b[39m"

#define LINK(url, text) "\e]8;;" url "\e\\" text "\e]8;;\e\\"

/*
 * give a string with %s where to input "text" and "url"
 */
#define LINK_FULL BLUE("\e]8;%s;\e\\%s\e]8;;\e\\")

#endif
