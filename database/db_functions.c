#include "db_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sqlite3.h>


int retrieve_id(void *, int, char **, char **);
int checkError(int , char *, sqlite3 *);


int get_client_id(char *name, sqlite3 *db, char *err_msg) {
    int rc, client_id;
    char *client_query = malloc(MAX_QUERY_SIZE);
    sprintf(client_query, "SELECT id FROM client WHERE client.name = '%s'", name);
    rc = sqlite3_exec(db, client_query, retrieve_id, &client_id, &err_msg);
    if (checkError(rc, err_msg, db) != 0) {
        free(client_query);
        return -1;
    }
    free(client_query);
    return client_id;
}

int get_showcase_id(char *movie, int day, int room, sqlite3 *db, char *err_msg) {
    int rc, showcase_id;
    char *showcase_query = malloc(MAX_QUERY_SIZE);
    sprintf(showcase_query,
            "SELECT id FROM showcase WHERE showcase.movie = '%s' AND showcase.room = %d AND showcase.day = %d", movie,
            room, day);
    rc = sqlite3_exec(db, showcase_query, retrieve_id, &showcase_id, &err_msg);
    if (checkError(rc, err_msg, db) != 0) {
        free(showcase_query);
        return -1;
    }
    free(showcase_query);
    return showcase_id;
}

int *consult(char *movie, int day, int room);

int book(char *movie, int day, int room, char *name, int seat) {
    sqlite3 *db;
    char *err_msg = ERR_MSG;

    int rc = sqlite3_open("cinema.db", &db);
    int client_id, showcase_id;

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    client_id = get_client_id(name, db, err_msg);
    if (client_id == -1) {
        return 1;
    }

    showcase_id = get_showcase_id(movie, day, room, db, err_msg);
    if (showcase_id == -1) {
        return 1;
    }


    char *insert_query = malloc(MAX_QUERY_SIZE);
    //TODO Add logic to not insert if it already exists
    sprintf(insert_query, "INSERT INTO booking VALUES(%d, %d, 0, %d)", client_id, showcase_id, seat);
    rc = sqlite3_exec(db, insert_query, 0, 0, &err_msg);
    if (checkError(rc, err_msg, db) != 0) {

        free(insert_query);

        return 1;
    }

    sqlite3_close(db);
    free(insert_query);

    return 0;
}

int cancel(char *movie, int day, int room, char *name, int seat) {
    sqlite3 *db;
    char *err_msg = ERR_MSG;
    int client_id, showcase_id;

    int rc = sqlite3_open("cinema.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    client_id = get_client_id(name, db, err_msg);
    if (client_id == -1) {
        return 1;
    }

    showcase_id = get_showcase_id(movie, day, room, db, err_msg);
    if (showcase_id == -1) {
        return 1;
    }

    char *update_query = malloc(MAX_QUERY_SIZE);
    sprintf(update_query,
            "UPDATE booking SET cancelled = 1 WHERE booking.client_id = '%d' AND booking.showcase_id = '%d' AND booking.seat = '%d'",
            client_id, showcase_id, seat);
    rc = sqlite3_exec(db,update_query,0,0,&err_msg);
    if(checkError(rc,err_msg,db)){
        free(update_query);
        return 1;
    }
    sqlite3_close(db);
    free(update_query);

    return 0;

}

int checkError(int rc, char *err_msg, sqlite3 *db) {
    if (rc != SQLITE_OK) {

        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return 1;
    }
    return 0;
}

int retrieve_id(void *data, int argc, char **argv,
                char **azColName) {


    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");

    int *ptr = (int *) data;
    *ptr = atoi(argv[0]);
    return 0;
}
