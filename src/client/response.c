#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include "response.h"
#include "../protocol.h"

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

/** Retorna una copia de arg */
char * copy(char * arg) {
    char * ret = malloc (strlen(arg) + 1);
    if (ret == NULL) {
        fprintf(stderr, "Memory error.");
        exit(EXIT_FAILURE);
    }
    strcpy(ret, arg);
    return ret;
}

void response_extract_seats(Response * response, int * seats) {

    assert(response->argc == SEATS);

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
        showcase->movie_name = copy(response->args[i]);
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
        ticket->showcase.movie_name = copy(response->args[i]);
        ticket->showcase.day   = atoi(response->args[i+1]);
        ticket->showcase.room  = atoi(response->args[i+2]);
        ticket->seat           = atoi(response->args[i+3]);
        i += 4;

        list_add(list, ticket);
    }

    return list;
}

Showcase * new_showcase(char * movie_name, int day, int room) {
    Showcase * showcase = malloc(sizeof(*showcase));
    if (showcase == NULL) {
        fprintf(stderr, "Memory error.");
        exit(EXIT_FAILURE);
    }

    showcase->movie_name = copy(movie_name);
    showcase->day = day;
    showcase->room = room;

    return showcase;
}

void destroy_showcase(Showcase * showcase) {
    free(showcase->movie_name);
    free(showcase);
}

Ticket * new_ticket(Showcase showcase, int seat) {
    Ticket * ticket = malloc(sizeof(*ticket));
    if (ticket == NULL) {
        fprintf(stderr, "Memory error.");
        exit(EXIT_FAILURE);
    }

    ticket->showcase.movie_name = copy(showcase.movie_name);
    ticket->showcase.day = showcase.day;
    ticket->showcase.room = showcase.room;
    ticket->seat = seat;

    return ticket;
}

void destroy_ticket(Ticket * ticket) {
    free(ticket->showcase.movie_name);
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
        case ALREADY_EXIST:
            ret = "ALREADY EXISTS";
            break;
        case FAIL_TO_OPEN:
            ret = "FAIL TO OPEN";
            break;
        case FAIL_QUERY:
            ret =  "FAIL QUERY";
            break;
        case BAD_BOOKING:
            ret = "BAD BOOKING";
            break;
        case BAD_CLIENT:
            ret =  "BAD CLIENT";
            break;
        case BAD_SHOWCASE:
            ret = "BAD SHOWCASE";
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
