#ifndef TPE_FINAL_SO_PROTOCOL_H
#define TPE_FINAL_SO_PROTOCOL_H

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

    SHOW_BOOKING,           // usuario              lista de reservados (movie, day, room, seat)
    SHOW_CANCELLED,         // usuario              lista de cancelados (movie, day, room, seat)

} request_type;

/**
 * TODO definir codigos de error
 * Posibles estados de la respuesta
 */
typedef enum {
    RESPONSE_OK = 0,
    RESPONSE_ERR,
} response_type;

#endif //TPE_FINAL_SO_PROTOCOL_H
