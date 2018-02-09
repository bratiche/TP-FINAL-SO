#include "request.h"

Request * parse_request(char * buffer) {

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


