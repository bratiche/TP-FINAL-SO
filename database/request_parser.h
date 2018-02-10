#ifndef TPE_FINAL_SO_REQUEST_PARSER_H
#define TPE_FINAL_SO_REQUEST_PARSER_H

#include "parser.h"

/**
 * Definicion del parser de la request
 */

/** eventos del parser */
enum request_event_type {
    /** 1 byte correspondiente al numero de comando */
        REQUEST_CMD,
    /** indica que hay un nuevo argumento para leer */
        REQUEST_NEWARG,
    /** 1 byte correspondiente al argumento */
        REQUEST_ARG,
    /** hay que esperar, no podemos decidir */
        REQUEST_WAIT,
    /** la request está completa */
        REQUEST_FIN,
};

/** retorna la definición del parser */
const ParserDefinition * request_parser_definition(void);

/** devuelve una descripcion del evento */
const char * request_event(enum request_event_type type);


#endif //TPE_FINAL_SO_REQUEST_PARSER_H
