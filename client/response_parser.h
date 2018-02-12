#ifndef TPE_FINAL_SO_RESPONSE_PARSER_H
#define TPE_FINAL_SO_RESPONSE_PARSER_H

#include <stdbool.h>
#include "../common/parser.h"

typedef struct {
    int status;
    int argc;
    char * args; // all arguments in one single string separated by \n
} Response;

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
    int args_size;
} ResponseParser;

/** Initializes the parser */
void response_parser_init(ResponseParser * parser);

/** Feeds a character to the parser, returns the new parser state */
response_state response_parser_feed(ResponseParser * parser, char c);

/**
 * Consumes the buffer feeding each character to the parser until the parsing is finished
 * or the buffer has nothing left to read, returns the new parser state.
 */
response_state response_parser_consume(ResponseParser * parser, char * buffer);

/** Returns true if the parsing is finished */
bool response_parser_is_done(ResponseParser * parser, bool *error);

//TODO ver donde se hace esto, no pareciera ser el trabajo del parser
/** Serializes the response into the buffer */
void response_parser_marshall(Response * response, char * buffer);

/** Frees resources */
void response_parser_destroy(ResponseParser * parser);

#endif //TPE_FINAL_SO_RESPONSE_PARSER_H
