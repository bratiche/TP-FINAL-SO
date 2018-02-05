#ifndef TP_FINAL_SO_DB_FUNCTIONS_H
#define TP_FINAL_SO_DB_FUNCTIONS_H

#include "stdbool.h"
#define SEATS 40

/*Returs array of seats, 1 if they're occupied, 0 if they're empty*/
int* consult(char* movie, int day, int sala);

/*Saves booking info on database*/
int book(char* movie, int day, int sala, char* name, int seat);

/*Cancels an existing booking*/
int cancel(char* movie, int day, int sala, char* name, int seat);

#endif //TP_FINAL_SO_DB_FUNCTIONS_H
