#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <mqueue.h>
#include "Comum.h"


/* abreviações :
mq - message queue
mqd+t = message queue descriptor type
attr= atributes
ped = pedidos
resp = respsotas
mq_maxmsg = message queue maximum messages
mq_msgsize = message queue message size

*/
int main (void){
    struct mq_attr attr_ped = {
        .mq_maxmsg = 10, 
        .mq_msgsize = sizeof(pedidoT)
    };
    struct mq_attr attr_resp = { 
        .mq_maxmsg = 10, 
        .mq_msgsize = sizeof(respostaT) 
    };

    //estabele canal de comunicação 
    mqd_t fila_ped  = mq_open(FILA_PEDIDOS,
        O_WRONLY | O_CREAT, //open write only | open & create
        0666,               //permissões em octal 
        &attr_ped);
    
    mqd_t fila_resp = mq_open(FILA_RESPOSTAS, 
        O_RDONLY | O_CREAT, //open read only | open  & create
        0666, 
        &attr_resp);
    if (fila_ped == (mqd_t)-1 || fila_resp == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    srandom(time(NULL) ^ getpid()); //resolve o problema do random() que não é realmente aleatório

    while (true) {
        pedidoT pedido;
        pedido.n1 = random() % 100;
        pedido.n2 = random() % 100;
 
        // envia n1 e n2 a B
        if (mq_send(fila_ped, (char *)&pedido, sizeof(pedido), 0) < 0) {
            perror("mq_send");
            exit(1);
        }
 
        // recebe soma de B
        respostaT resp;
        if (mq_receive(fila_resp, (char *)&resp, sizeof(resp), NULL) < 0) {
            perror("mq_receive");
            exit(1);
        }
 
        // escreve n1 + n2 = soma (marca quando a soma nao confere)
        printf("[A %d] %d + %d = %d%s\n", getpid(), pedido.n1, pedido.n2, resp.soma,
               (pedido.n1 + pedido.n2 == resp.soma) ? "" : "   <-- ERRADO!");
        fflush(stdout);
    }
    return 0;
}