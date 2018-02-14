#ifndef TPE_FINAL_SO_RESPONSE_H
#define TPE_FINAL_SO_RESPONSE_H

#include "../common/list.h"

#define ROWS        10
#define COLS        4
#define SEATS_COUNT ROWS * COLS

typedef struct {
    int status;
    int argc;
    char ** args;
} Response;

typedef struct {
    char * movie_name;
    int day;
    int room;
} Showcase;

typedef struct {
    Showcase showcase;
    int seat;
} Ticket;

Response * new_response(void);

/** Fills the array with 1 if the seat is reserved and 0 if it is not */
void response_extract_seats(Response * response, int * seats);

/** Returns a list of movie names */
List response_extract_movies(Response * response);

/** Returns a list of Showcases */
List response_extract_showcases(Response * response);

/** Returns a list of Tickets */
List response_extract_tickets(Response * response);

char * get_response_status(Response * response);

void print_response(Response * response);

void destroy_response(Response * response);

#endif //TPE_FINAL_SO_RESPONSE_H
