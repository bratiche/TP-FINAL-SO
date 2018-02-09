#ifndef TPE_FINAL_SO_REQUEST_H
#define TPE_FINAL_SO_REQUEST_H

#define MAX_ARGS 5

typedef enum {
                            // argumentos           respuesta

    ADD_CLIENT,             // name                 ok

    ADD_SHOWCASE,           // showcase             ok o err
    REMOVE_SHOWCASE,        // showcase             ok o err

    GET_MOVIES,             // -                    lista de peliculas (strings)
    GET_SHOWCASES,          // nombre de pelicula   lista de showcases (movie, day, room)

    GET_SEATS,              // movie, day, room     array binario

    ADD_BOOKING,            // usuario, movie, day, room, seat      ok o err
    REMOVE_BOOKING,         // usuario, movie, day, room, seat      ok o err

    SHOW_BOOKING,           // usuario              lista de reservados (movie, day, room, seat)
    SHOW_CANCELLED,         // usuario              lista de cancelados (movie, day, room, seat)

} request_type;

typedef struct {
    request_type type;
    char * args[MAX_ARGS];
} Request;

Request * parse_request(char * buffer);

void process_request(Request * request, char ** buffer);

/**
 * EJEMPLOS:
 *
 *  crear reserva (request):
 *
 *  6
 *  juan ramirez perez
 *  la gran pelicula maravillosa
 *  4
 *  4
 *  2
 *
 *  mostrar reservados(response):
 *
 *  ok/err
 *  movie
 *  day
 *  room
 *  seat
 *  movie
 *  day
 *  room
 *  seat
 *  .
 *
 *  ok
 *  .
 *
 */

/**
 * paquete de request:
 * REQ_ID \n ARGUMENTOS separados por \n
 *
 * paquete response:
 * OK/ERR \n DATOS separados por \n . \n
 *
 *
 */


#endif //TPE_FINAL_SO_REQUEST_H
