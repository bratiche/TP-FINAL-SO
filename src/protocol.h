#ifndef TPE_FINAL_SO_PROTOCOL_H
#define TPE_FINAL_SO_PROTOCOL_H

#define ROWS        10
#define COLS        8
#define SEATS       ROWS * COLS

#define MAX_ARGS    5
#define ARG_SIZE    50
#define ROOMS       5

#define MOVIE_NAME_LENGTH   ARG_SIZE
#define CLIENT_NAME_LENGTH  ARG_SIZE
#define EMPTY_SEAT          1
#define RESERVED_SEAT       0


/**
 * Protocolo de comunicacion orientado a texto.
 *
 * request:
 * REQ_TYPE \n ARGUMENTOS separados por \n . \n
 *
 * response:
 * RES_TYPE \n DATOS separados por \n . \n
 */

/**
 * Comandos de la request
 */
typedef enum {
    // comando              argumentos              respuesta

    ADD_CLIENT,             // name                 ok

    ADD_SHOWCASE,           // movie, day, room     ok o err
    REMOVE_SHOWCASE,        // movie, day, room     ok o err

    GET_MOVIES,             // -                    lista de peliculas (strings)
    GET_SHOWCASES,          // nombre de pelicula   lista de showcases (movie, day, room)

    GET_SEATS,              // movie, day, room     array binario

    ADD_BOOKING,            // usuario, movie, day, room, seat      ok o err
    REMOVE_BOOKING,         // usuario, movie, day, room, seat      ok o err

    GET_BOOKING,            // usuario               lista de reservados (movie, day, room, seat)
    GET_CANCELLED,          // usuario               lista de cancelados (movie, day, room, seat)

} request_type;

/**
 * Posibles estados de la respuesta
 */
typedef enum {
    RESPONSE_OK = 0,
    RESPONSE_ERR,
    ALREADY_EXIST,
    FAIL_TO_OPEN,
    FAIL_QUERY,
    BAD_BOOKING,
    BAD_CLIENT,
    BAD_SHOWCASE
} response_type;

typedef enum {
    SUN,
    MON,
    TUE,
    WED,
    THU,
    FRI,
    SAT,
} day;

/** Funciones auxiliares para traducir los enums a strings */

char * get_day(int day);

char * get_request_type(int type);

char * get_response_type(int type);

#endif //TPE_FINAL_SO_PROTOCOL_H
