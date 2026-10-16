// fila.h - fila circular de inteiros com capacidade fixa
#ifndef FILA_H
#define FILA_H

#define CAPACIDADE 10

typedef struct {
    int dados[CAPACIDADE];
    int inicio;    // posicao do primeiro elemento
    int fim;      // posicao onde entra o proximo elemento
    int tamanho;   // quantidade de elementos na fila
} fila_t;

void fila_inicia(fila_t *f);
int  fila_retira(fila_t *f, int *valor);  // retorna 0 se ok, -1 se vazia
int  fila_poe(fila_t *f, int valor);      // retorna 0 se ok, -1 se cheia
void fila_imprime(fila_t *f);

#endif
