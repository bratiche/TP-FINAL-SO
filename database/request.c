#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include "db_functions.h"
#include "request.h"
#include "request_parser.h"

bool debug = false;

static void print_state(const char *p, const char *(*namefnc)(unsigned), const ParserEvent *e) {
    if (e->n == 0) {
        fprintf(stderr, "%-8s: %-14s\n", p, namefnc(e->type));
    } else {
        for (int j = 0; j < e->n; j++) {
            const char *name = (j == 0) ? namefnc(e->type)
                                        : "";
            if (e->data[j] <= ' ') {
                fprintf(stderr, "%-8s: %-14s 0x%02X\n", p, name, e->data[j]);
            } else {
                fprintf(stderr, "%-8s: %-14s %c\n", p, name, e->data[j]);
            }
        }
    }
}

Request * new_request(void) {
    Request * request = malloc(sizeof(*request));
    if (request == NULL) {
        return NULL;
    }

    request->type  = 0;
    request->argc  = 0;
    request->state = request_cmd;

    for (int i = 0; i < MAX_ARGS; i++) {
        request->args[i] = calloc(ARG_SIZE, sizeof(char));
        if (request->args[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(request->args[j]);
            }
            free(request);
            return NULL;
        }
    }

    return request;
}

request_state cmd(Request * request, const ParserEvent * e) {
    if (!isdigit(e->data[0])) {
        return request_error_invalid_cmd;
    }

    request->type *= 10;
    request->type += e->data[0] - '0';
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

request_state newarg(Request * request, char c, int j) {
    request->argc++;

    request_state ret;
    if (request->argc > MAX_ARGS) {
        ret = request_error_too_many_arguments;
    } else {
        ret = arg(request, c, j);
    }

    return ret;
}

Request * parse_request(char * buffer) {
    Request * ret = new_request();
    if (ret == NULL) {
        return NULL;
    }

    const ParserDefinition * def = request_parser_definition();
    Parser *request_parser = parser_init(parser_no_classes(), def);
    int j = 0;

    while (*buffer != 0 && ret->state < request_done) {
        uint8_t c = (uint8_t) *buffer++;
        const ParserEvent *e = parser_feed(request_parser, c);
        do {
            if (debug) print_state("request", request_event, e);
            switch (e->type) {
                case REQUEST_CMD:
                    ret->state = cmd(ret, e);
                    break;
                case REQUEST_NEWARG:
                    j = 0;
                    ret->state = newarg(ret, e->data[0], j++);
                    break;
                case REQUEST_ARG:
                    ret->state = arg(ret, e->data[0], j++);
                    break;
                case REQUEST_WAIT:
                    // nada por hacer mas que esperar
                    break;
                case REQUEST_FIN:
                    ret->state = request_done;
                    break;
                default:
                    ret->state = request_error;
                    break;
            }
            e =  e->next;
        } while (e != NULL);
    }

    parser_destroy(request_parser);

    return ret;
}

void process_request(Request * request, char * buffer) {
    switch(request->type){
        case ADD_CLIENT:
            break;
        case ADD_SHOWCASE:
            break;
        case ADD_BOOKING:
            break;
        case GET_MOVIES:
            break;
        case GET_SEATS:
            break;
        case GET_SHOWCASES:
            break;
        case SHOW_BOOKING:
            break;
        case SHOW_CANCELLED:
            break;
        case REMOVE_BOOKING:
            break;
        case REMOVE_SHOWCASE:
            break;
        default:
            //TODO: when this s blow
            break; //remove it after

    }
}

char * get_cmd(int type) {
    char * ret;
    switch(type) {
        case ADD_CLIENT:
            ret = "add client";
            break;
        case ADD_SHOWCASE:
            ret = "add showcase";
            break;
        case ADD_BOOKING:
            ret = "add booking";
            break;
        case REMOVE_SHOWCASE:
            ret = "remove showcase";
            break;
        case REMOVE_BOOKING:
            ret = "remove booking";
            break;
        case SHOW_BOOKING:
            ret = "show booking";
            break;
        case SHOW_CANCELLED:
            ret = "show cancelled";
            break;
        case GET_SHOWCASES:
            ret = "get showcases";
            break;
        case GET_MOVIES:
            ret = "get movies";
            break;
        case GET_SEATS:
            ret = "get seats";
            break;
        default:
            ret = "UNKNOWN COMMAND";
            break;
    }

    return ret;
}

void print_request(Request * request) {
    if (request->state != request_done) {
        printf("BAD REQUEST\n");
    }

    printf("argc:%d\ncmd:%d\ntype:%s\n", request->argc, request->type, get_cmd(request->type));

    for (int i = 0; i < request->argc; i++) {
        printf("argv[%d]:%s\n", i, request->args[i]);
    }

}

void destroy_request(Request * request) {
    for (int i = 0; i < MAX_ARGS; i++) {
        free(request->args[i]);
    }
    free(request);
}


