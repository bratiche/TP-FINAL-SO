#include "db_functions.h"
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

#define BUFFER_SIZE 4098

int main(int argc, char const *argv[]) {

    printf("%s\n",sqlite3_libversion());

    if (database_init() < 0) {
        return -1;
    }

    book("Shrek",2,4,"tobias",19);
    cancel("Shrek",2,4,"tobias",19);

    char buffer[BUFFER_SIZE];
    ssize_t n;

    do {
        bzero(buffer, BUFFER_SIZE);
        n = read(STDIN_FILENO, buffer, BUFFER_SIZE);
        if (n > 0) {
            write(STDOUT_FILENO, buffer, strlen(buffer));
        }
    } while (n > 0);

    return 0;
}