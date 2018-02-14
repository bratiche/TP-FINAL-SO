#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "response_parser.h"
#include "../multiline_parser.h"
#include "../utils.h"

#define ARG_BLOCK   50

extern bool debug;

void response_parser_init(ResponseParser * parser, Response * response) {
    const ParserDefinition *definition = multiline_parser_definition();
    parser->multiline_parser = parser_init(parser_no_classes(), definition);
    parser->response = response;
    parser->state = response_status;
    parser->arg_index = 0;
    parser->arg = NULL;
}

response_state new_arg_block(ResponseParser * parser) {
    char * aux = parser->arg;

    aux = realloc(aux, (size_t)parser->arg_index + ARG_BLOCK);
    if (aux == NULL) {
        return response_error;
    }

    aux[parser->arg_index] = 0;
    parser->arg = aux;
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
            parser->arg[parser->arg_index++] = c;
            if (parser->arg_index % ARG_BLOCK == 0) {
                ret = new_arg_block(parser);
            }
            break;
        default:
            ret = response_error;
            break;
    }

    return ret;
}

response_state copy_arg(ResponseParser * parser, response_state state) {
    char ** aux = realloc(parser->response->args, (sizeof (char*)) * (parser->response->argc + 1));

    if (aux == NULL) {
        fprintf(stderr, "Memory error");
        return response_error;
    }

    char * aux_arg = calloc((size_t)parser->arg_index + 1, sizeof(char));
    if (aux_arg == NULL) {
        free(aux);
        fprintf(stderr, "Memory error");
        return response_error;
    }

    strncpy(aux_arg, parser->arg, (size_t)parser->arg_index);
    parser->response->args = aux;
    parser->response->args[parser->response->argc++] = aux_arg;

    return state;
}

response_state newline(ResponseParser * parser, char c) {
    response_state ret = response_args;

    // copio el arg viejo si lo habia
    if (parser->arg != NULL) {
        if (copy_arg(parser, ret) == response_error) {
            return response_error;
        }
        free(parser->arg);
        parser->arg_index = 0;
        parser->arg = NULL;
    }

    ret = new_arg_block(parser);
    parser->arg[parser->arg_index++] = c;

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
                // copy last arg
                if (parser->arg != NULL) {
                    parser->state = copy_arg(parser, response_done);
                } else {
                    parser->state = response_done;
                }
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

void response_parser_destroy(ResponseParser * parser) {
    parser_destroy(parser->multiline_parser);
    free(parser->arg);
}
