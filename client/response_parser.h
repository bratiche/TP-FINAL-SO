#ifndef TPE_FINAL_SO_RESPONSE_PARSER_H
#define TPE_FINAL_SO_RESPONSE_PARSER_H

#include <stdbool.h>
#include "response.h"
#include "../common/parser.h"

typedef enum {
    response_status,
    response_args,
    response_done,
    response_error,
} response_state;

typedef struct {
    Parser * multiline_parser;
    Response * response;
    response_state state;
    char * arg;
    int arg_index;
} ResponseParser;

/** Initializes the parser */
void response_parser_init(ResponseParser * parser, Response * response);

/** Feeds a character to the parser, returns the new parser state */
response_state response_parser_feed(ResponseParser * parser, char c);

/**
 * Consumes the buffer feeding each character to the parser until the parsing is finished
 * or the buffer has nothing left to read, returns the new parser state.
 */
response_state response_parser_consume(ResponseParser * parser, char * buffer);

/** Returns true if the parsing is finished */
bool response_parser_is_done(ResponseParser * parser, bool *error);

/** Frees resources */
void response_parser_destroy(ResponseParser * parser);

#endif //TPE_FINAL_SO_RESPONSE_PARSER_H
