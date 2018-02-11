#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include "db_functions.h"
#include "request.h"

#define BUFFER_SIZE 4096

int main(int argc, char const *argv[]) {
    printf("%s\n",sqlite3_libversion());
    database_open();
    add_client("MACRI");
    int prueba=get_client_id("MACRI gatoASD");
    printf("%d\n",prueba);
    database_close();

    if (database_init() < 0) {
        return -1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t n;

    do {
        bzero(buffer, BUFFER_SIZE);
        n = read(STDIN_FILENO, buffer, BUFFER_SIZE);

        Request * request = parse_request(buffer);
        print_request(request);
        process_request(request, buffer);
        destroy_request(request);

        if (n > 0) {
            write(STDOUT_FILENO, buffer, strlen(buffer));
        }
    } while (n > 0);

    return 0;
}