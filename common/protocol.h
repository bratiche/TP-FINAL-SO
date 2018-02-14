#ifndef TPE_FINAL_SO_PROTOCOL_H
#define TPE_FINAL_SO_PROTOCOL_H

#define ROWS        10
#define COLS        4
#define SEATS       ROWS * COLS

#define MOVIE_NAME_LENGTH 50
#define ROOMS             5

/**
 * TODO explicar el protocolo
 * request:
 * REQ_ID \n ARGUMENTOS separados por \n . \n
 *
 * response:
 * OK/ERR \n DATOS separados por \n . \n
 */

/**
 * Comandos de la request
 */
typedef enum {
    // argumentos           respuesta

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
 * TODO definir codigos de error
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
} days;

#endif //TPE_FINAL_SO_PROTOCOL_H
