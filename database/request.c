#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "request.h"
#include "../common/protocol.h"
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

void process_request(int state, Request * request, char * buffer) {

    if (state != request_done) {
        // send error
        sprintf(buffer, "%d\n.\n", RESPONSE_ERR);
        write(STDOUT_FILENO, buffer, strlen(buffer));
        return;
    }

    print_request(request);

//
//    write(STDOUT_FILENO, buffer, strlen(buffer));

    int cache;
    int seats[40]={0};
    switch(request->type){ //TODO VERIFICAR QUE NO ROMPE
        case ADD_CLIENT:
            cache=add_client(request->args[0]);
            printf("%d\n", cache);
            break;
        case ADD_SHOWCASE:
            cache=add_showcase(request->args[0],atoi(request->args[1]),atoi(request->args[2]));
            printf("%d\n", cache);
            break;
        case ADD_BOOKING:
            cache=add_booking(request->args[0],atoi(request->args[1]),atoi(request->args[2]),request->args[3],atoi(request->args[4]));
            printf("%d\n",cache);
            break;
        case GET_MOVIES:
            printf("%d\n",OK);
            cache=show_movies();
            break;
        case GET_SEATS:
            printf("%d\n",OK);
            cache=show_seats(request->args[0],atoi(request->args[1]),atoi(request->args[2]));
            for(int i;i<SEATS;i++){
                printf("%d\n",seats[i]);
            }
            break;
        case GET_SHOWCASES:
            printf("%d\n",0);
            cache=show_showcases();
            break;
        case SHOW_BOOKING:
            printf("%d\n",OK);
            cache=show_client_booking(request->args[0]);
            break;
        case SHOW_CANCELLED:
            printf("%d\n",OK);
            cache=show_client_cancelled(request->args[0]);
            break;
        case REMOVE_BOOKING:
            cache=cancel_booking(request->args[0],atoi(request->args[1]),atoi(request->args[2]),request->args[3],atoi(request->args[4]));
            printf("%d\n",cache);
            break;
        case REMOVE_SHOWCASE:
            cache=remove_showcase(request->args[0],atoi(request->args[1]),atoi(request->args[2]));
            printf("%d\n",cache);
            break;
        default:
            break; //remove it after

    }
    // send_ok


    write(STDOUT_FILENO, ".\n", strlen(buffer));
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


