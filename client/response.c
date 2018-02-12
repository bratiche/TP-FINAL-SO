#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "response.h"
#include "../common/protocol.h"

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

void process_response(Response * response) {

}

#define DELIM "\n"

List response_extract_movies(Response * response) {
    List list = list_new();

    char * args = response->args;
    char * token = strtok(args, DELIM);

    while (token != NULL) {
        list_add(list, token);
        token = strtok(args, DELIM);
    }

    return list;
}

// TODO test
List response_extract_showcases(Response * response) {
    List list = list_new();

    char * args = response->args;
    char * token = strtok(args, DELIM);

    while (token != NULL) {
        Showcase * showcase = malloc(sizeof(*showcase));
        if (showcase == NULL) {
            fprintf(stderr, "Memory error.");
            exit(EXIT_FAILURE);
        }
        showcase->movie_name = token;
        token = strtok(args, DELIM);
        if (token != NULL) {
            showcase->day = atoi(token);
            token = strtok(args, DELIM);
            if (token != NULL) {
                showcase->room = atoi(token);
            }
        }
        list_add(list, showcase);
        token = strtok(args, DELIM);
    }

    return list;
}

char * get_response_status(Response * response) {
    char * ret;
    switch (response->status) {
        case RESPONSE_OK:
            ret = "OK";
            break;
        case RESPONSE_ERR:
            ret = "ERROR";
            break;
        default:
            ret = "UNKNOWN ERROR";
            break;
    }

    return ret;
}

void print_response(Response * response) {
    printf("status:%d\n", response->status);
    printf("argc:%d\n", response->argc);
    printf("args:\n%s\n", response->args);
}

void destroy_response(Response * response) {
    free(response->args);
    free(response);
}
