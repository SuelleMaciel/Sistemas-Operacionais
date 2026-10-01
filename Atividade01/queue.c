#include <stdio.h>
#include "queue.h"

#include <stdbool.h>

int queue_size (queue_t *queue) {
    if (queue == NULL) {
        return 0;
    }

    //ponteiro da head da lista
    queue_t *aux = queue ;
    int contador = 0;

    do {
        contador++;
        aux = aux -> next;
    } while (aux != queue); //só para quando der a volta

    return contador;
}

void queue_print (char *name, queue_t *queue, void print_elem (void*)) {
    printf("%s: [", name);

    if (queue == NULL) {
        printf("]\n");
        return;
    }

    queue_t *aux = queue;
    do {
        print_elem (aux);
        aux = aux -> next;
        if (aux != queue) {
            printf(" ");
        }
    } while (aux != queue);
    printf("]\n");
}

int queue_append (queue_t **queue, queue_t *elem) {
    if (queue == NULL) {
        fprintf(stderr, "A fila nao existe\n");
        return -1;
    }

    if (elem == NULL) {
        fprintf(stderr, "O elemento nao existe\n");
        return -1;
    }

    if (elem->next != NULL || elem->prev != NULL) {
        fprintf(stderr, "O elemento esta em outra fila.\n");
        return -1;
    }

    if (*queue == NULL) { // elemento sozinho
        elem -> next = elem;
        elem -> prev = elem;
        *queue = elem;
    }

    else {
        queue_t *ultimo = (*queue)->prev; //o último é o anterior do primeiro
        ultimo -> next = elem;
        elem -> prev = ultimo;
        elem -> next = *queue;
        (*queue)->prev = elem;
    }

    return 0;
}

int queue_remove (queue_t **queue, queue_t *elem) {
    if (queue == NULL) {
        fprintf(stderr, "A fila nao existe\n");
        return -1;
    }
    if (*queue == NULL) {
        fprintf(stderr, "Nao e possivel remover elementos de uma fila vazia\n");
        return -1;
    }
    if (elem == NULL) {
        fprintf(stderr, "O elemento nao existe\n");
        return -1;
    }

    queue_t *aux = *queue;
    bool achou = false;
    do{
        if (aux == elem) {
            achou = true;
            break;
        }
        aux = aux -> next;
    } while (aux != *queue);

    if (achou == false) {
        fprintf(stderr, "O elemento nao pertence a fila\n");
        return -1;
    }
    if (elem->next == elem && elem->prev == elem) {
        *queue = NULL;
    }
    else {
        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;

        if (elem == *queue) {
            *queue = elem->next;
        }
    }
    elem->prev = NULL;
    elem->next = NULL;
    return 0;
}