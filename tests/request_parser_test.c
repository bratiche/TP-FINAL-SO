
#include <assert.h>
#include "../src/database/request_parser.h"

void parse(char * buffer);

void request_parser_test() {
    parse("0\nusername\n.\n");
    parse("6\njuan ramirez perez\n"
          "la gran pelicula maravillosa\n"
          "4\n"
          "4\n"
          "2\n"
          "."
          "\n");
}

void parse(char * buffer) {
    RequestParser parser;

    request_parser_init(&parser);
    request_parser_consume(&parser, buffer);
    assert(request_parser_is_done(&parser, 0) == true);
    print_request(parser.request);
    request_parser_destroy(&parser);
}