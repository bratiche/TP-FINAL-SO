#include <assert.h>
#include <stdio.h>
#include "response_parser_test.h"
#include "../client/response_parser.h"

void parse2(char * buffer);

void response_parser_test() {
    parse2("0\nmovie 1\nmovie 2\nmovie 3\n.\n");

}

void print_response(Response * response) {
    printf("status:%d\n", response->status);
    printf("argc:%d\n", response->argc);
    printf("args:\n%s\n", response->args);
}

void parse2(char * buffer) {

    ResponseParser parser;

    response_parser_init(&parser);
    response_parser_consume(&parser, buffer);
    assert(response_parser_is_done(&parser, 0) == true);
    print_response(parser.response);
    response_parser_destroy(&parser);
}