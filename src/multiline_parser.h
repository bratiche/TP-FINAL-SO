#ifndef TPE_FINAL_SO_MULTILINE_PARSER_H
#define TPE_FINAL_SO_MULTILINE_PARSER_H

#include "parser.h"

/**
 * Definicion de un parser multilinea donde el mensaje termina con \n . \n
 */

/** eventos del parser */
enum multiline_event_type {
    /** 1 byte para leer */
        MULTI_BYTE,
    /** nueva linea */
        MULTI_NEWLINE,
    /** hay que esperar, no podemos decidir */
        MULTI_WAIT,
    /** el mensaje esta terminado */
        MULTI_FIN,
};

/** retorna la definición del parser */
const ParserDefinition * multiline_parser_definition(void);

/** devuelve una descripcion del evento */
const char * multiline_event(enum multiline_event_type type);

#endif //TPE_FINAL_SO_MULTILINE_PARSER_H
