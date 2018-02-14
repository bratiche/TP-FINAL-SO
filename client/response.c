#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include "response.h"
#include "../common/protocol.h"

Response * new_response() {
    Response * ret = malloc(sizeof(*ret));
    if (ret == NULL) {
        return NULL;
    }

    ret->status = RESPONSE_OK;
    ret->argc = 0;
    ret->args = NULL;

    return ret;
}

void response_extract_seats(Response * response, int * seats) {

    assert(response->argc == SEATS_COUNT);

    for (int i = 0; i < response->argc; i++) {
        seats[i] = atoi(response->args[i]);
    }
}

List response_extract_movies(Response * response) {
    List list = list_new();

    for (int i = 0; i < response->argc; i++) {
        list_add(list, response->args[i]);
    }

    return list;
}

// TODO test
List response_extract_showcases(Response * response) {
    List list = list_new();

    for (int i = 0; i < response->argc;) {
        if (response->argc < i + 3) {
            fprintf(stderr, "Response error.");
            exit(EXIT_FAILURE);
        }
        Showcase * showcase = malloc(sizeof(*showcase));
        if (showcase == NULL) {
            fprintf(stderr, "Memory error.");
            exit(EXIT_FAILURE);
        }
        showcase->movie_name = response->args[i];
        showcase->day   = atoi(response->args[i+1]);
        showcase->room  = atoi(response->args[i+2]);
        i += 3;

        list_add(list, showcase);
    }

    return list;
}

List response_extract_tickets(Response * response) {
    List list = list_new();

    for (int i = 0; i < response->argc;) {
        if (response->argc < i + 4) {
            fprintf(stderr, "Response error.");
            exit(EXIT_FAILURE);
        }

        Ticket * ticket = malloc(sizeof(*ticket));
        if (ticket == NULL) {
            fprintf(stderr, "Memory error.");
            exit(EXIT_FAILURE);
        }
        ticket->showcase.movie_name = response->args[i];
        ticket->showcase.day   = atoi(response->args[i+1]);
        ticket->showcase.room  = atoi(response->args[i+2]);
        ticket->seat           = atoi(response->args[i+3]);
        i += 4;

        list_add(list, ticket);
    }

    return list;
}

void destroy_showcase(Showcase * showcase) {
    free(showcase);
}

void destroy_ticket(Ticket * ticket) {
    free(ticket);
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
    for (int i = 0; i < response->argc; i++) {
        printf("argv[%d]: %s\n", i + 1, response->args[i]);
    }
}

void destroy_response(Response * response) {
    for (int i = 0; i < response->argc; i++) {
        free(response->args[i]);
    }
    free(response->args);
    free(response);
}
