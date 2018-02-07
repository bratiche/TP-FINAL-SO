#include "db_functions.h"
#include <sqlite3.h>
#include <stdio.h>

//TODO esto esta copypasteado del codigo de git
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

char * filename = "cinema.db";

int main(int argc, char const *argv[]) {

    printf("%s\n",sqlite3_libversion());

    sqlite3 * db;
    char * err_msg = ERR_MSG;

    if (sqlite3_open(filename, &db) != SQLITE_OK) {
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    if (sqlite3_exec(db, create_tables, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to create database tables. SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    sqlite3_close(db);

    book("Shrek",2,4,"tobias",19);
    cancel("Shrek",2,4,"tobias",19);

    return 0;
}