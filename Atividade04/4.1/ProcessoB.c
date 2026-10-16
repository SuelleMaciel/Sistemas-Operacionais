// processoB.c - recebe pares de numeros de A e devolve a soma
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include "Comum.h"

int main(void)
{
    struct mq_attr attr_ped  = { .mq_maxmsg = 10, .mq_msgsize = sizeof(pedidoT) };
    struct mq_attr attr_resp = { .mq_maxmsg = 10, .mq_msgsize = sizeof(respostaT) };

    // estabelece o canal de comunicacao
    mqd_t fila_ped  = mq_open(FILA_PEDIDOS,   O_RDONLY | O_CREAT, 0666, &attr_ped);
    mqd_t fila_resp = mq_open(FILA_RESPOSTAS, O_WRONLY | O_CREAT, 0666, &attr_resp);
    if (fila_ped == (mqd_t)-1 || fila_resp == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    while (1) {
        // recebe n1 e n2 de A
        pedidoT pedido;
        if (mq_receive(fila_ped, (char *)&pedido, sizeof(pedido), NULL) < 0) {
            perror("mq_receive");
            exit(1);
        }

        respostaT resp;
        resp.soma = pedido.n1 + pedido.n2;
        printf("[B %d] recebi %d e %d, devolvendo %d\n", getpid(), pedido.n1, pedido.n2, resp.soma);
        fflush(stdout);

        sleep(1);  // aguarda 1s

        // envia soma a A
        if (mq_send(fila_resp, (char *)&resp, sizeof(resp), 0) < 0) {
            perror("mq_send");
            exit(1);
        }
    }
    return 0;
}