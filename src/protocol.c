#include "protocol.h"

char * get_day(int day) {
    char * ret;
    switch (day) {
        case SUN:
            ret = "SUN";
            break;
        case MON:
            ret = "MON";
            break;
        case TUE:
            ret = "TUE";
            break;
        case WED:
            ret = "WED";
            break;
        case THU:
            ret = "THU";
            break;
        case FRI:
            ret = "FRI";
            break;
        case SAT:
            ret = "SAT";
            break;
        default:
            ret = "GG";
            break;
    }

    return ret;
}

char * get_request_type(int type) {
    char * ret;
    switch(type) {
        case ADD_CLIENT:
            ret = "ADD_CLIENT";
            break;
        case ADD_SHOWCASE:
            ret = "ADD_SHOWCASE";
            break;
        case ADD_BOOKING:
            ret = "ADD_BOOKING";
            break;
        case REMOVE_SHOWCASE:
            ret = "REMOVE_SHOWCASE";
            break;
        case REMOVE_BOOKING:
            ret = "REMOVE_BOOKING";
            break;
        case GET_BOOKING:
            ret = "GET_BOOKING";
            break;
        case GET_CANCELLED:
            ret = "GET_CANCELLED";
            break;
        case GET_SHOWCASES:
            ret = "GET_SHOWCASES";
            break;
        case GET_MOVIES:
            ret = "GET_MOVIES";
            break;
        case GET_SEATS:
            ret = "GET_SEATS";
            break;
        default:
            ret = "UNKNOWN COMMAND";
            break;
    }

    return ret;
}

char * get_response_type(int type) {
    char * ret;
    switch (type) {
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