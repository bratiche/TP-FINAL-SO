#ifndef TPE_FINAL_SO_RESPONSE_H
#define TPE_FINAL_SO_RESPONSE_H

#include "../common/list.h"

#define ARGS_BLOCK  50   // used for memory allocation

typedef struct {
    int status;
    int argc;
    char * args; // all arguments in one single string separated by \n
} Response;

typedef struct {
    char * movie_name;
    int day;
    int room;
} Showcase;

Response * new_response(void);

void process_response(Response * response);

/** Returns a list of movie names */
List response_extract_movies(Response * response);

List response_extract_showcases(Response * response);

char * get_response_status(Response * response);

void print_response(Response * response);

void destroy_response(Response * response);

#endif //TPE_FINAL_SO_RESPONSE_H
