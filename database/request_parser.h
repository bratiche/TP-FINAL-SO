#ifndef TPE_FINAL_SO_REQUEST_PARSER_H
#define TPE_FINAL_SO_REQUEST_PARSER_H

#include <stdbool.h>
#include "../common/parser.h"
#include "request.h"

typedef enum {
    request_cmd,
    request_args,
    request_done,
    request_error,
    request_error_invalid_cmd,
    request_error_argument_too_long,
    request_error_too_many_arguments,
} request_state;

typedef struct {
    Parser * multiline_parser;
    Request * request;
    request_state state;
    int arg_index;
} RequestParser;

/** Initializes the parser */
void request_parser_init(RequestParser * parser);

/** Feeds a character to the parser, returns the new parser state */
request_state request_parser_feed(RequestParser * parser, char c);

/**
 * Consumes the buffer feeding each character to the parser until the parsing is finished
 * or the buffer has nothing left to read, returns the new parser state.
 */
request_state request_parser_consume(RequestParser * parser, char * buffer);

/** Returns true if the parsing is finished */
bool request_parser_is_done(RequestParser * parser, bool *error);

/** Frees resources */
void request_parser_destroy(RequestParser * parser);

#endif //TPE_FINAL_SO_REQUEST_PARSER_H
