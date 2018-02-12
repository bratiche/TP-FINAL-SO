#include <assert.h>
#include "response_parser_test.h"
#include "../client/response_parser.h"

void parse2(char * buffer);

void response_parser_test() {
    parse2("0\nmovie 1\nmovie 2\nmovie 3\n.\n");

}

void parse2(char * buffer) {

    ResponseParser parser;
    Response * response = new_response();
    response_parser_init(&parser, response);
    response_parser_consume(&parser, buffer);
    assert(response_parser_is_done(&parser, 0) == true);
    print_response(parser.response);
    response_parser_destroy(&parser);
    destroy_response(response);
}