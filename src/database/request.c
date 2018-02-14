#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <syslog.h>
#include "request.h"
#include "../protocol.h"
#include "db_functions.h"
#include "request_parser.h"

extern bool debug;

Request * new_request(void) {
    Request * request = malloc(sizeof(*request));
    if (request == NULL) {
        return NULL;
    }

    request->type  = 0;
    request->argc  = 0;

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

static void log_request(Request * request) {
    char buffer[BUFFER_SIZE];
    char * aux = buffer;
    aux += sprintf(aux, "%s(", get_request_type(request->type));

    for (int i = 0; i < request->argc; i++) {
        aux += sprintf(aux, "%s", request->args[i]);
        if (i < request->argc -1) {
            aux += sprintf(aux, ",");
        }
    }

    sprintf(aux, ")");
    syslog(LOG_DEBUG, "[DATABASE] request %s", buffer);
}

static void log_response(int type) {
    syslog(LOG_DEBUG, "[DATABASE] response %s", get_response_type(type));
}

void process_request(int state, Request * request) {

    if (state != request_done) {
        // send error
        printf("%d\n.\n", RESPONSE_ERR);
        fflush(stdout);
        return;
    }
    log_request(request);
    database_open();

    int cache = RESPONSE_ERR;
    switch(request->type){
        case ADD_CLIENT:
            cache=add_client(request->args[0]);
            printf("%d\n", cache);
            break;
        case ADD_SHOWCASE:
            cache=add_showcase(request->args[0],atoi(request->args[1]),atoi(request->args[2]));
            printf("%d\n", cache);
            break;
        case ADD_BOOKING:
            cache=add_booking(request->args[0],request->args[1],atoi(request->args[2]),atoi(request->args[3]),atoi(request->args[4]));
            printf("%d\n",cache);
            break;
        case GET_MOVIES:
            cache = show_movies();
            break;
        case GET_SEATS:
            cache = show_seats(request->args[0],atoi(request->args[1]),atoi(request->args[2]));
            break;
        case GET_SHOWCASES:
            cache = show_showcases_by_movie(request->args[0]);
            break;
        case GET_BOOKING:
            cache = show_client_booking(request->args[0]);
            break;
        case GET_CANCELLED:
            cache = show_client_cancelled(request->args[0]);
            break;
        case REMOVE_BOOKING:
            cache=cancel_booking(request->args[0],request->args[1],atoi(request->args[2]),atoi(request->args[3]),atoi(request->args[4]));
            printf("%d\n",cache);
            break;
        case REMOVE_SHOWCASE:
            cache=remove_showcase(request->args[0],atoi(request->args[1]),atoi(request->args[2]));
            printf("%d\n",cache);
            break;
        default:
            printf("%d\n",RESPONSE_ERR);
            break; //remove it after

    }

    printf(".\n");
    fflush(stdout);     // la magia
    database_close();
    log_response(cache);
}

void print_request(Request * request) {
    printf("argc:%d\ncmd:%d\ntype:%s\n", request->argc, request->type, get_request_type(request->type));

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


