#ifndef TP_FINAL_SO_DB_FUNCTIONS_H
#define TP_FINAL_SO_DB_FUNCTIONS_H

#include <sqlite3.h>
#include "stdbool.h"
#define DB_FILENAME "cinema.db"
#define SEATS 40
#define ERR_MSG 0
#define MAX_QUERY_SIZE 500
#define STR_BLOCK_SIZE 100


#define UNSPECIFIED_ERROR -1
#define INVALID_ID -1

typedef enum{
        OK,
        ALREADY_EXIST,
        FAIL_TO_OPEN,
        FAIL_QUERY,
        BAD_BOOKING,
        BAD_CLIENT,
        BAD_SHOWCASE

} return_type;

//TODO: AGREGADOS DE OTA
int database_open();
int database_close();

int add_client(char *name);
int add_showcase(char *movie, int day, int room);

int show_client_booking(char* name,char **str);
int show_client_cancelled(char* name,char **str);

int get_client_id(char *name);
int get_showcase_id(char *movie, int day, int room);



/*Returns array of seats, 1 if they're occupied, 0 if they're empty*/
//int* consult(char* movie, int day, int sala);

/*Saves booking info on database*/
int add_booking(char *movie, int day, int sala, char *name, int seat);

/*Cancels an existing booking*/
int cancel_booking(char *movie, int day, int sala, char *name, int seat);

#endif //TP_FINAL_SO_DB_FUNCTIONS_H
