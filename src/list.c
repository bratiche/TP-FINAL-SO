#include <stdio.h>
#include <stdlib.h>

#include "list.h"

struct list {
    struct list_node *first, *last;
    struct list_node *current;   //usado para recorrer la lista
    int 			  size;
};

struct list_node {
    void *data;
    struct list_node *next;
};

struct list * list_new() {
    struct list *ret = malloc(sizeof(*ret));

    if (ret == NULL) {
        exit(EXIT_FAILURE);
    }

    ret->first      = ret->last = NULL;
    ret->current    = NULL;
    ret->size       = 0;

    return ret;
}

static struct list_node * new_node(void *data) {
    struct list_node *ret = malloc(sizeof(*ret));

    if (ret == NULL) {
        exit(EXIT_FAILURE);
    }

    ret->data = data;
    ret->next = NULL;

    return ret;
}

void list_add(struct list *l, void *data) {
    struct list_node *last = l->last;

    if (data == NULL) {
        return;
    }

    if (last == NULL) {
        last = new_node(data);	// lista vacia
        l->first = l->last = last;
        l->current = l->first;  // seteo el puntero para recorrer
    } else {
        last->next = new_node(data);
        l->last = last->next;
    }

    l->size++;
}

void * list_get(struct list *list, int index) {
    if (index < 0 || index >= list->size) {
        return NULL;
    }

    struct list_node * ret = list->first;

    while (index-- > 0) {
        ret = ret->next;
    }

    return ret->data;
}

bool list_is_empty(struct list *l) {
    return l->size == 0;
}

int list_size(struct list *l) {
    return l->size;
}

void * list_get_next(struct list *l) {
    void * ret;

    if (l->current != NULL) {
        ret = l->current->data;
        l->current = l->current->next;
    } else {    // termine de recorrer o la lista estaba vacia
        l->current = l->first;
        ret = NULL;
    }

    return ret;
}

void list_destroy(struct list *l) {
    struct list_node *first = l->first;
    struct list_node *aux;

    while (first != NULL) {
        aux = first->next;
        free(first);
        first = aux;
    }

    free(l);
}
