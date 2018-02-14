#include <ctype.h>
#include "request_parser.h"
#include "../multiline_parser.h"
#include "../utils.h"

extern bool debug;

void request_parser_init(RequestParser * parser) {
    const ParserDefinition *definition = multiline_parser_definition();
    parser->multiline_parser = parser_init(parser_no_classes(), definition);
    parser->request = new_request();
    if (parser->request == NULL) {
        parser_destroy(parser->multiline_parser);
        // ERROR
    }
    parser->arg_index = 0;
    parser->state = request_cmd;
}

request_state cmd(Request * request, char c) {
    if (!isdigit(c)) {
        return request_error_invalid_cmd;
    }

    request->type *= 10;
    request->type += c - '0';
    return request_cmd;
}

request_state arg(Request * request, char c, int j) {
    request_state ret = request_args;
    if (j >= ARG_SIZE - 1) {
        ret = request_error_argument_too_long;
    } else {
        request->args[request->argc - 1][j] = c;
    }

    return ret;
}

request_state req_byte(RequestParser * parser, char c) {
    request_state ret;
    switch (parser->state) {
        case request_cmd:
            ret = cmd(parser->request, c);
            break;
        case request_args:
            ret = arg(parser->request, c, parser->arg_index++);
            break;
        default:
            ret = request_error;
            break;
    }

    return ret;
}

request_state req_newline(RequestParser * parser, char c) {
    parser->arg_index = 0;
    parser->request->argc++;

    request_state ret;
    if (parser->request->argc > MAX_ARGS) {
        ret = request_error_too_many_arguments;
    } else {
        ret = arg(parser->request, c, parser->arg_index++);
    }

    return ret;
}

request_state request_parser_feed(RequestParser * parser, char c) {
    const ParserEvent *e = parser_feed(parser->multiline_parser, (uint8_t) c);
    do {
        if (debug) print_state("request", multiline_event, e);
        switch (e->type) {
            case MULTI_BYTE:
                parser->state = req_byte(parser, e->data[0]);
                break;
            case MULTI_NEWLINE:
                parser->state = req_newline(parser, e->data[0]);
                break;
            case MULTI_WAIT:
                // nada por hacer mas que esperar
                break;
            case MULTI_FIN:
                parser->state = request_done;
                break;
            default:
                parser->state = request_error;
                break;
        }
        e =  e->next;
    } while (e != NULL && parser->state < request_done);

    return parser->state;
}

request_state request_parser_consume(RequestParser * parser, char * buffer) {
    request_state state = parser->state;

    while (*buffer != 0) {
        state = request_parser_feed(parser, *buffer++);
        if (request_parser_is_done(parser, 0)) {
            break;
        }
    }

    return state;
}

bool request_parser_is_done(RequestParser * parser, bool *error) {
    if (parser->state >= request_error && error != 0) {
        *error = true;
    }

    return parser->state >= request_done;
}

void request_parser_destroy(RequestParser * parser) {
    parser_destroy(parser->multiline_parser);
    destroy_request(parser->request);
}


