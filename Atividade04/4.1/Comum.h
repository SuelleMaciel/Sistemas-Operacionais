// o que vai ser compartilhado entre processo A e B
#ifndef COMUM_H
#define COMUM_H
#include <mqueue.h> // dá erro no windows, mas se usar o wsl pra rodar, por exemplo, funciona.

#define FILA_PEDIDOS "/fila-suelle-pedidos"
#define FILA_RESPOSTAS "/fila-suelle-respostas"

typedef struct {
    int n1;
    int n2;
} pedidoT;

typedef struct {
    int soma;
} respostaT;

#endif // COMUM_H