#include "db_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char * create_tables =
        "CREATE TABLE IF NOT EXISTS client(\n"
                "\tid INTEGER NOT NULL,\n"
                "\tname TEXT,\n"
                "\tPRIMARY KEY(id)\n"
                ");\n"
                "\n"
                "CREATE TABLE IF NOT EXISTS showcase(\n"
                "\tid INTEGER NOT NULL,\n"
                "\tmovie TEXT NOT NULL,\n"
                "\tday INT NOT NULL,\n"
                "\troom INT NOT NULL,\n"
                "\tPRIMARY KEY(id)\n"
                ");\n"
                "\n"
                "CREATE TABLE IF NOT EXISTS booking(\n"
                "\tclient_id INTEGER NOT NULL,\n"
                "\tshowcase_id INTEGER NOT NULL,\n"
                "\tcancelled INTEGER NOT NULL,\n"
                "\tseat INTEGER NOT NULL,\n"
                "\tFOREIGN KEY (client_id) REFERENCES client(id),\n"
                "\tFOREIGN KEY (showcase_id) REFERENCES showcase(id),\n"
                "\tPRIMARY KEY (client_id, showcase_id, seat)\n"
                ");";

char * filename = "cinema.db"; //TODO ELIMINAR
sqlite3* db_fd;
char* exec_error_msg="Ocurrio un error en la ejecucion\n"; //todo despues tiene que ser NULL




int callback_dummy(void *data, int argc, char **argv, char **azColName) {return 0;}


//ES EL QUE INICIALIZA LA DB_FD
int database_open(){
    int file_exist=access(DB_FILENAME,F_OK);
    if (sqlite3_open(DB_FILENAME, &db_fd) != SQLITE_OK) {
        sqlite3_close(db_fd);
        return FAIL_TO_OPEN;
    }
    if(file_exist==-1) {
        if (sqlite3_exec(db_fd, create_tables, NULL, NULL, NULL) != SQLITE_OK)
            return FAIL_QUERY;
    }
    return OK;
}

int database_close(){
    sqlite3_close(db_fd);
    return OK;
}

int add_client(char *name){
    //TODO VERIFICAR SI EXISTE EL CLIENTE
    char *insert_query = malloc(MAX_QUERY_SIZE);
    sprintf(insert_query, "INSERT INTO client(name) VALUES('%s')", name);
    int rc = sqlite3_exec(db_fd, insert_query, 0, 0, &exec_error_msg);
    free(insert_query);
    if(rc!=SQLITE_OK)
        return FAIL_QUERY;
    return OK;
}

int add_showcase(char *movie, int day, int room) {
    //TODO VERIFICAR SI EXISTE EL SHOWCASE
    char *insert_query = malloc(MAX_QUERY_SIZE);
    sprintf(insert_query, "INSERT INTO showcase(movie,day,room) VALUES('%s',%d,%d)", movie,day,room);
    int rc = sqlite3_exec(db_fd, insert_query, 0, 0, &exec_error_msg);
    free(insert_query);
    if(rc!=SQLITE_OK)
        return FAIL_QUERY;
    return OK;
}

int get_client_id(char *name) {
    int rc, client_id=INVALID_ID; //En caso que sean 0 tuplas retorna INVALID_ID
    char *client_query = malloc(MAX_QUERY_SIZE);
    sprintf(client_query, "SELECT id FROM client WHERE client.name = '%s'", name);
    rc = sqlite3_exec(db_fd, client_query, callback_retr_id, &client_id, &exec_error_msg);
    free(client_query);
//    if (rc != SQLITE_OK)
//        return FAIL_QUERY;
    return client_id;
}

int get_showcase_id(char *movie, int day, int room) {
    int rc, showcase_id=INVALID_ID; //En caso que estan 0 tuplas retorna INVALID_ID
    char *showcase_query = malloc(MAX_QUERY_SIZE);
    sprintf(showcase_query,
            "SELECT id FROM showcase WHERE showcase.movie = '%s' AND showcase.room = %d AND showcase.day = %d", movie,
            room, day);
    rc = sqlite3_exec(db_fd, showcase_query, callback_retr_id, &showcase_id, &exec_error_msg);
    free(showcase_query);
//    if (rc != SQLITE_OK)
//        return FAIL_QUERY;
    return showcase_id;
}

int* show_booking(char* movie,int day,int room){
    int rc,show_id=get_showcase_id(movie,day,room);
    if(show_id == INVALID_ID) {
        return NULL;
    }
    int* seats=calloc(SEATS* sizeof(int),0);
    for(int i=0;i<SEATS;i++){
        int client_id=INVALID_ID;
        char *showb_query=malloc(MAX_QUERY_SIZE);
        sprintf(showb_query,"SELECT client_id FROM booking WHERE showcase_id = %d AND seat = %d AND cancelled = 0",show_id,i);
        rc = sqlite3_exec(db_fd,showb_query,callback_dummy,&client_id,&exec_error_msg);
        free(showb_query);
        if(client_id==INVALID_ID){
            seats[i]=1;
        }
    }
    return seats;
}


//TODO que es consult?
//int *consult(char *movie, int day, int room);

int add_booking(char *movie, int day, int room, char *name, int seat) {
    sqlite3 *db=db_fd;
    char *err_msg = ERR_MSG;
    int rc;
//    int rc = sqlite3_open("cinema.db", &db);
    int client_id, showcase_id;
//
//    if (rc != SQLITE_OK) {
//
//        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
//        sqlite3_close(db);
//
//        return 1;
//    }


    client_id = get_client_id(name);
    if (client_id == -1) {
        return 1;
    }

    showcase_id = get_showcase_id(movie, day, room);
    if (showcase_id == -1) {
        return 1;
    }


    char *insert_query = malloc(MAX_QUERY_SIZE);
    //TODO Add logic to not insert if it already exists
    sprintf(insert_query, "INSERT INTO booking VALUES(%d, %d, 0, %d)", client_id, showcase_id, seat);
    rc = sqlite3_exec(db, insert_query, 0, 0, &err_msg);
    if (check_error(rc, err_msg, db) != 0) {

        free(insert_query);

        return 1;
    }

    sqlite3_close(db);
    free(insert_query);

    return 0;
}

int cancel_booking(char *movie, int day, int room, char *name, int seat) {
    sqlite3 *db;
    char *err_msg = ERR_MSG;
    int client_id, showcase_id;

    int rc = sqlite3_open("cinema.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    client_id = get_client_id(name);
    if (client_id == -1) {
        return 1;
    }

    showcase_id = get_showcase_id(movie, day, room);
    if (showcase_id == -1) {
        return 1;
    }

    char *update_query = malloc(MAX_QUERY_SIZE);
    sprintf(update_query,
            "UPDATE booking SET cancelled = 1 WHERE booking.client_id = '%d' AND booking.showcase_id = '%d' AND booking.seat = '%d'",
            client_id, showcase_id, seat);
    rc = sqlite3_exec(db,update_query,0,0,&err_msg);
    if(check_error(rc, err_msg, db)){
        free(update_query);
        return 1;
    }
    sqlite3_close(db);
    free(update_query);

    return 0;

}

//TODO DEPRECATED
int check_error(int rc, char *err_msg, sqlite3 *db) {
    if (rc != SQLITE_OK) {

        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return 1;
    }
    return 0;
}

//TODO DEPRECATED
int database_init() {

    sqlite3 * db;
    char * err_msg = ERR_MSG;

    if (sqlite3_open(filename, &db) != SQLITE_OK) {
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return UNSPECIFIED_ERROR;
    }

    if (sqlite3_exec(db, create_tables, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to create database tables. SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return UNSPECIFIED_ERROR;
    }

    sqlite3_close(db);
    return OK;
}



int callback_retr_id(void *data, int argc, char **argv, char **azColName) {

    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");

    int *ptr = (int *) data;
    *ptr = atoi(argv[0]);
    return 0;
}
