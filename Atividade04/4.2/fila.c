// fila.c - implementacao da fila circular
#include <stdio.h>
#include "fila.h"

void fila_inicia(fila_t *f)
{
    f->inicio = 0;
    f->fim = 0;
    f->tamanho = 0;
}

int fila_retira(fila_t *f, int *valor)
{
    if (f->tamanho <= 0)
        return -1;
    *valor = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % CAPACIDADE;
    f->tamanho--;
    return 0;
}

int fila_poe(fila_t *f, int valor)
{
    if (f->tamanho >= CAPACIDADE)
        return -1;
    f->dados[f->fim] = valor;
    f->fim = (f->fim + 1) % CAPACIDADE;
    f->tamanho++;
    return 0;
}

void fila_imprime(fila_t *f)
{
    // limita a impressao caso o tamanho tenha sido corrompido
    int n = f->tamanho;
    if (n < 0) n = 0;
    if (n > CAPACIDADE) n = CAPACIDADE;

    for (int i = 0; i < n; i++)
        printf("%d ", f->dados[(f->inicio + i) % CAPACIDADE]);
}
