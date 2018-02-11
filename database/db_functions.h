#ifndef TP_FINAL_SO_DB_FUNCTIONS_H
#define TP_FINAL_SO_DB_FUNCTIONS_H

#include <sqlite3.h>
#include "stdbool.h"
#define SEATS 40
#define ERR_MSG 0
#define MAX_QUERY_SIZE 500

#define UNSPECIFIED_ERROR -1

typedef enum{
        OK,
        ALREADY_EXIST,


} return_type;

int database_init();


//TODO: AGREGADOS DE OTA
int add_client(char *name);


//TODO: DEFINE THIS PLZ
int retrieve_id(void *, int, char **, char **);
int check_error(int, char *, sqlite3 *);

/*Returns array of seats, 1 if they're occupied, 0 if they're empty*/
int* consult(char* movie, int day, int sala);

/*Saves booking info on database*/
int book(char* movie, int day, int sala, char* name, int seat);

/*Cancels an existing booking*/
int cancel(char* movie, int day, int sala, char* name, int seat);

#endif //TP_FINAL_SO_DB_FUNCTIONS_H
