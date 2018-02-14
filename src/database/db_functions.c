#include "db_functions.h"
#include "../protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

int callback_dummy(void *data, int argc, char **argv, char **azColName) {
    return 0;
}

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

sqlite3* db_fd;
char* exec_error_msg="Modifica exec\n"; //todo despues tiene que ser NULL
int callback_retr_id(void *data, int argc, char **argv, char **azColName);



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
    return RESPONSE_OK;
}

int database_close(){
    sqlite3_close(db_fd);
    return RESPONSE_OK;
}

int add_client(char *name){
    int client_id=get_client_id(name);
    if(client_id!=INVALID_ID)
        return ALREADY_EXIST;
    char *insert_query = malloc(MAX_QUERY_SIZE);
    sprintf(insert_query, "INSERT INTO client(name) VALUES('%s')", name);
    int rc = sqlite3_exec(db_fd, insert_query, 0, 0, &exec_error_msg);
    free(insert_query);
    if(rc!=SQLITE_OK)
        return FAIL_QUERY;
    return RESPONSE_OK;
}

int add_showcase(char *movie, int day, int room) {
    int showcase_id=get_showcase_id(movie,day,room);
    if(showcase_id!=INVALID_ID)
        return ALREADY_EXIST;
    char *insert_query = malloc(MAX_QUERY_SIZE);
    sprintf(insert_query, "INSERT INTO showcase(movie,day,room) VALUES('%s',%d,%d)",
            movie,day,room);
    int rc = sqlite3_exec(db_fd, insert_query, 0, 0, &exec_error_msg);
    free(insert_query);
    if(rc!=SQLITE_OK)
        return FAIL_QUERY;
    return RESPONSE_OK;
}

int remove_showcase(char *movie, int day, int room) {
    int showcase_id=get_showcase_id(movie,day,room);
    if(showcase_id==INVALID_ID)
        return BAD_SHOWCASE;
    char *insert_query = malloc(MAX_QUERY_SIZE);
    sprintf(insert_query, "DELETE FROM booking WHERE showcase_id = %d", showcase_id);
    int rc = sqlite3_exec(db_fd, insert_query, 0, 0, &exec_error_msg);
    free(insert_query);
    if(rc!=SQLITE_OK)
        return FAIL_QUERY;
    insert_query = malloc(MAX_QUERY_SIZE);
    sprintf(insert_query, "DELETE FROM showcase WHERE movie='%s' AND day=%d AND room=%d",
            movie,day,room);
    rc = sqlite3_exec(db_fd, insert_query, 0, 0, &exec_error_msg);
    free(insert_query);
    if(rc!=SQLITE_OK)
        return FAIL_QUERY;
    return RESPONSE_OK;
}

int get_client_id(char *name) {
    int rc, client_id=INVALID_ID; //En caso que sean 0 tuplas retorna INVALID_ID
    char *client_query = malloc(MAX_QUERY_SIZE);
    sprintf(client_query, "SELECT id FROM client WHERE name = '%s'", name);
    rc = sqlite3_exec(db_fd, client_query, callback_retr_id, &client_id, NULL);
    free(client_query);
    return client_id;
}

int get_showcase_id(char *movie, int day, int room) {
    int rc, showcase_id=INVALID_ID; //En caso que estan 0 tuplas retorna INVALID_ID
    char *showcase_query = malloc(MAX_QUERY_SIZE);
    sprintf(showcase_query,
            "SELECT id FROM showcase WHERE showcase.movie = '%s' AND showcase.room = %d AND showcase.day = %d",
            movie, room, day);
    rc = sqlite3_exec(db_fd, showcase_query, callback_retr_id, &showcase_id, NULL);
    free(showcase_query);
    if (rc != SQLITE_OK)
        return FAIL_QUERY;
    return showcase_id;
}

int print_cols(int rc,sqlite3_stmt *stmt){

    int rowCount = 0;
    const unsigned char * textCol=0;
    rc = sqlite3_step(stmt);
    while (rc != SQLITE_DONE && rc != SQLITE_OK)
    {
        rowCount++;
        int colCount = sqlite3_column_count(stmt);
        for (int colIndex = 0; colIndex < colCount; colIndex++)
        {
            int type = sqlite3_column_type(stmt, colIndex);
            const char * columnName = sqlite3_column_name(stmt, colIndex);
            textCol = sqlite3_column_text(stmt, colIndex);
            printf("%s\n",textCol);
        }
        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
    return RESPONSE_OK;
}

int show_movies(){
    sqlite3_stmt *stmt = NULL;
    char* showq=malloc(MAX_QUERY_SIZE);
    sprintf(showq,"SELECT DISTINCT movie FROM showcase");
    int rc = sqlite3_prepare_v2(db_fd, showq, -1, &stmt, NULL);
    free(showq);
    if (rc != SQLITE_OK) {
        printf("%d\n", FAIL_QUERY);
        return FAIL_QUERY;
    }

    printf("%d\n", RESPONSE_OK);
    print_cols(rc,stmt);
    return RESPONSE_OK;
}

int show_showcases(){
    sqlite3_stmt *stmt = NULL;
    char* showq=malloc(MAX_QUERY_SIZE);
    sprintf(showq,"SELECT DISTINCT movie,day,room FROM showcase");
    int rc = sqlite3_prepare_v2(db_fd, showq, -1, &stmt, NULL);
    free(showq);
    if (rc != SQLITE_OK) {
        printf("%d\n", FAIL_QUERY);
        return FAIL_QUERY;
    }

    printf("%d\n", RESPONSE_OK);
    print_cols(rc,stmt);
    return RESPONSE_OK;
}

int show_client_booking(char* name){
    const unsigned char * textCol=0;
    sqlite3_stmt *stmt = NULL;
    int client_id = get_client_id(name);
    if (client_id == INVALID_ID) {
        printf("%d\n", BAD_CLIENT);
        return BAD_CLIENT;
    }
    printf("%d\n", RESPONSE_OK);

    char* showq=malloc(MAX_QUERY_SIZE);
    sprintf(showq,"SELECT movie,day,room,seat FROM booking INNER JOIN showcase ON showcase.id = booking.showcase_id "
                    "WHERE client_id = %d AND cancelled = 0",
            client_id);
    int rc = sqlite3_prepare_v2(db_fd, showq, -1, &stmt, NULL);
    free(showq);
    if (rc != SQLITE_OK) {
        printf("%d\n", FAIL_QUERY);
        return FAIL_QUERY;
    }

    print_cols(rc,stmt);
    return RESPONSE_OK;
}

int show_client_cancelled(char* name){
    const unsigned char * textCol=0;
    sqlite3_stmt *stmt = NULL;
    int client_id = get_client_id(name);
    if (client_id == INVALID_ID) {
        printf("%d\n",BAD_CLIENT);
        return BAD_CLIENT;
    }
    printf("%d\n", RESPONSE_OK);

    char* showq=malloc(MAX_QUERY_SIZE);
    sprintf(showq,"SELECT movie,day,room,seat FROM booking INNER JOIN showcase ON showcase.id = booking.showcase_id "
            "WHERE client_id = %d AND cancelled = 1",
            client_id);
    int rc = sqlite3_prepare_v2(db_fd, showq, -1, &stmt, NULL);
    free(showq);
    if (rc != SQLITE_OK) {
        printf("%d\n", FAIL_QUERY);
        return FAIL_QUERY;
    }
    print_cols(rc,stmt);
    return RESPONSE_OK;
}

int show_seats(char *movie, int day, int room){
    int rc,show_id=get_showcase_id(movie,day,room);
    if(show_id == INVALID_ID) {
        printf("%d\n",BAD_SHOWCASE);
        return BAD_SHOWCASE;
    }
    printf("%d\n", RESPONSE_OK);

    for(int i=0;i<SEATS;i++){
        int client_id=INVALID_ID;
        char *showb_query=malloc(MAX_QUERY_SIZE);
        sprintf(showb_query,"SELECT client_id FROM booking "
                "WHERE showcase_id = %d AND seat = %d AND cancelled = 0",
                show_id,i);
        rc = sqlite3_exec(db_fd,showb_query,callback_retr_id,&client_id,&exec_error_msg);
        free(showb_query);
        if(client_id==INVALID_ID){
            printf("1\n");
        }else{
            printf("0\n");
        }
    }
    return RESPONSE_OK;
}

int add_booking(char *name, char *movie, int day, int room, int seat) {
    char *err_msg = ERR_MSG;
    int rc;
    int client_id, showcase_id;

    client_id = get_client_id(name);
    if (client_id == INVALID_ID) {
        return BAD_CLIENT;
    }

    showcase_id = get_showcase_id(movie, day, room);
    if (showcase_id == INVALID_ID) {
        return BAD_SHOWCASE;
    }
    int exist=INVALID_ID; //En caso que sean 0 tuplas retorna INVALID_ID
    char *client_query = malloc(MAX_QUERY_SIZE);
    sprintf(client_query, "SELECT client_id FROM booking "
                    "WHERE client_id = %d AND showcase_id = %d AND seat = %d "
                    "AND cancelled = 0",
            client_id,showcase_id,seat);
    rc = sqlite3_exec(db_fd, client_query, callback_retr_id, &exist, NULL);
    free(client_query);
    if(exist!=INVALID_ID)
        return ALREADY_EXIST;
    char *insert_query = malloc(MAX_QUERY_SIZE);

    sprintf(insert_query, "INSERT INTO booking VALUES(%d, %d, 0, %d)", client_id, showcase_id, seat);
    rc = sqlite3_exec(db_fd, insert_query, NULL, NULL, NULL);
    free(insert_query);
    if (rc != SQLITE_OK)
        return FAIL_QUERY;
    return RESPONSE_OK;
}

int cancel_booking(char *name, char *movie, int day, int room, int seat) {
    int client_id, showcase_id, rc;

    client_id = get_client_id(name);
    if (client_id == INVALID_ID) {
        return BAD_CLIENT;
    }

    showcase_id = get_showcase_id(movie, day, room);
    if (showcase_id == INVALID_ID) {
        return BAD_SHOWCASE;
    }

    char *update_query = malloc(MAX_QUERY_SIZE);
    sprintf(update_query,
            "UPDATE booking SET cancelled = 1 WHERE booking.client_id = '%d' AND booking.showcase_id = '%d' AND booking.seat = '%d'",
            client_id, showcase_id, seat);
    rc = sqlite3_exec(db_fd,update_query,NULL,NULL,NULL);
    free(update_query);
    if (rc != SQLITE_OK){
        return FAIL_QUERY;
    }
    return RESPONSE_OK;

}

int callback_retr_id(void *data, int argc, char **argv, char **azColName) {
    int *ptr = (int *) data;
    *ptr = atoi(argv[0]);
    return 0;
}
