#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "response_parser.h"
#include "../common/multiline_parser.h"
#include "../common/utils.h"
#include "../common/protocol.h"

#define ARGS_BLOCK  50   // length of the arguments string

extern bool debug;

Response * new_response() {
    Response * ret = malloc(sizeof(*ret));
    if (ret == NULL) {
        return NULL;
    }

    ret->status = RESPONSE_OK;
    ret->argc = 0;
    ret->args = calloc(ARGS_BLOCK, sizeof(char));
    if (ret->args == NULL) {
        free(ret);
        return NULL;
    }

    return ret;
}

void destroy_response(Response * response) {
    free(response->args);
    free(response);
}

void response_parser_init(ResponseParser * parser) {
    const ParserDefinition *definition = multiline_parser_definition();
    parser->multiline_parser = parser_init(parser_no_classes(), definition);
    parser->response = new_response();
    if (parser->response == NULL) {
        parser_destroy(parser->multiline_parser);
        // ERROR
    }
    parser->args_size = 0;
    parser->state = response_status;
}

response_state new_args_block(ResponseParser * parser) {
    char * aux = parser->response->args;

    aux = realloc(aux, (size_t)parser->args_size + ARGS_BLOCK);
    if (aux == NULL) {
        return response_error;
    }

    aux[parser->args_size] = 0;
    parser->response->args = aux;
    return response_args;
}

response_state byte(ResponseParser * parser, char c) {
    response_state ret;

    switch (parser->state) {
        case response_status:
            if (!isdigit(c)) {
                ret = response_error;
            } else {
                ret = response_status;
                parser->response->status *= 10;
                parser->response->status += c - '0';
            }
            break;
        case response_args:
            ret = response_args;
            parser->response->args[parser->args_size++] = c;
            if (parser->args_size % ARGS_BLOCK == 0) {
                ret = new_args_block(parser);
            }
            break;
        default:
            ret = response_error;
            break;
    }

    return ret;
}

response_state newline(ResponseParser * parser, char c) {
    response_state ret = response_args;

    if (parser->response->argc > 0) {
        parser->response->args[parser->args_size++] = '\n';
        if (parser->args_size % ARGS_BLOCK == 0) {
            if (new_args_block(parser) == response_error) {
                return response_error;
            }
        }
    }

    parser->response->argc++;
    parser->response->args[parser->args_size++] = c;
    if (parser->args_size % ARGS_BLOCK == 0) {
        ret = new_args_block(parser);
    }
    return ret;
}

response_state response_parser_feed(ResponseParser * parser, char c) {
    const ParserEvent * e = parser_feed(parser->multiline_parser, (uint8_t) c);
    do {
        if (debug) print_state("response", multiline_event, e);
        switch (e->type) {
            case MULTI_BYTE:
                parser->state = byte(parser, e->data[0]);
                break;
            case MULTI_NEWLINE:
                parser->state = newline(parser, e->data[0]);
                break;
            case MULTI_WAIT:
                //
                break;
            case MULTI_FIN:
                parser->state = response_done;
                break;
            default:
                parser->state = response_error;
                break;
        }
        e = e->next;
    } while (e != NULL && parser->state < response_done);

    return parser->state;
}

response_state response_parser_consume(ResponseParser * parser, char * buffer) {
    response_state state = parser->state;

    while (*buffer != 0) {
        state = response_parser_feed(parser, *buffer++);
        if (response_parser_is_done(parser, 0)) {
            break;
        }
    }

    return state;
}

bool response_parser_is_done(ResponseParser * parser, bool *error) {
    if (parser->state >= response_error && error != 0) {
        *error = true;
    }

    return parser->state >= response_done;
}


/** Serializes the response into the buffer */
void response_parser_marshall(Response * response, char * buffer) {

}

void response_parser_destroy(ResponseParser * parser) {
    parser_destroy(parser->multiline_parser);
    destroy_response(parser->response);
}
