#ifndef TP_FINAL_SO_DB_FUNCTIONS_H
#define TP_FINAL_SO_DB_FUNCTIONS_H

#include <sqlite3.h>
#include "stdbool.h"
#include "../protocol.h"

#define ERR_MSG 0
#define MAX_QUERY_SIZE (2 + MAX_ARGS  * (ARG_SIZE + 2))

#define INVALID_ID (-1)

int database_open(const char * filename);
int database_close();

int add_client(char *name);
int add_showcase(char *movie, int day, int room);
int remove_showcase(char *movie, int day, int room);

int show_movies();
int show_showcases(char* movie);
int show_client_booking(char* name);
int show_client_cancelled(char* name);
int show_seats(char *movie, int day, int room);

int get_client_id(char *name);
int get_showcase_id(char *movie, int day, int room);

/*Saves booking info on database*/
int add_booking(char *name, char *movie, int day, int sala, int seat);

/*Cancels an existing booking*/
int cancel_booking(char *name, char *movie, int day, int sala, int seat);

#endif //TP_FINAL_SO_DB_FUNCTIONS_H
