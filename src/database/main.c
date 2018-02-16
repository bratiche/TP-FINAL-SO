#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <syslog.h>
#include <stdlib.h>
#include "db_functions.h"
#include "request.h"
#include "request_parser.h"

void process_request(int state, Request * request);

int main(int argc, char const *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    if (database_open(argv[1]) != RESPONSE_OK) {
        fprintf(stderr, "Error opening database '%s'", argv[1]);
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t n;
    do {
        RequestParser parser;
        request_parser_init(&parser);
        bool done = false;
        do {
            bzero(buffer, BUFFER_SIZE);
            n = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            if (n > 0) {
                request_parser_consume(&parser, buffer);
                done = request_parser_is_done(&parser, 0);
            }
        } while(n > 0 && !done);

        process_request(parser.state, parser.request);
        request_parser_destroy(&parser);
    } while (n > 0);

    database_close();

    return 0;
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
            cache = show_showcases(request->args[0]);
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
    log_response(cache);
}