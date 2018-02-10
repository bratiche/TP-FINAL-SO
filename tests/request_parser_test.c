
#include "../database/request.h"

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
    Request * request = parse_request(buffer);
    print_request(request);
    destroy_request(request);
}