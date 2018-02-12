#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include "db_functions.h"
#include "request.h"
#include "request_parser.h"

#define BUFFER_SIZE 4096

int main(int argc, char const *argv[]) {
    database_open(); //TODO VERIFICAR SI SE ROMPE

    char buffer[BUFFER_SIZE];
    ssize_t n;

    do {
        RequestParser parser;
        request_parser_init(&parser);
        bool done = false;
        do {
            bzero(buffer, BUFFER_SIZE);
            n = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            if (n > 0) {
                request_parser_consume(&parser, buffer);
                done = request_parser_is_done(&parser, 0);
            }
        } while(n > 0 && !done);

        print_request(parser.request);
        process_request(parser.request, buffer);
        request_parser_destroy(&parser);
    } while (n > 0);

    database_close();
    return 0;
}