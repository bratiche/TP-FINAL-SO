#ifndef TPE_FINAL_SO_LIST_H
#define TPE_FINAL_SO_LIST_H

#include <stdbool.h>

/**
 * Lista simplemente encadenada que soporta insercion al final
 * y provee un metodo para recorrerla
 */

typedef struct list * List;

struct list * list_new(void);

/** Agrega un elemento al final de la lista */
void list_add(struct list *list, void *data);

void * list_get(struct list *list, int index);

bool list_is_empty(struct list *list);

int list_size(struct list *list);

/**
 * Se usa para recorrer la lista iterativamente, retorna NULL cuando no hay mas elementos.
 * Si la lista no estaba vacia, la siguiente llamada a una llamada que retorno NULL retorna
 * el primer elemento de la lista (Se resetea el recorrido).
 */
void * list_get_next(struct list *list);

void list_destroy(struct list *list);

#endif //TPE_FINAL_SO_LIST_H
