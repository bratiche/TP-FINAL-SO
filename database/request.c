#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "request.h"
#include "../common/protocol.h"
#include "db_functions.h"

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

void process_request(Request * request, char * buffer) {
    int cache;
    switch(request->type){ //TODO VERIFICAR QUE NO ROMPE
        case ADD_CLIENT:
            if(request->argc==1){
                add_client(request->args[0]);
            }
            break;
        case ADD_SHOWCASE:
            if(request->argc==3){
                add_showcase(request->args[0],atoi(request->args[1]),atoi(request->args[2]));
            }
            break;
        case ADD_BOOKING:
            cache=add_booking(request->args[0],atoi(request->args[1]),atoi(request->args[2]),request->args[3],atoi(request->args[4]));
            printf("%d\n",cache);
            break;
        case GET_MOVIES:
            break;
        case GET_SEATS:
            break;
        case GET_SHOWCASES:
            break;
        case SHOW_BOOKING:
            cache=show_client_booking(request->args[0],NULL);
            printf("%d\n",cache);
            break;
        case SHOW_CANCELLED:
            break;
        case REMOVE_BOOKING:
            break;
        case REMOVE_SHOWCASE:
            break;
        default:
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


