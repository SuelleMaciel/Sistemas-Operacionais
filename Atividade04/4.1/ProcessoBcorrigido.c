// correção: - responde na fila particular do A que fez o pedido
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

#define FILA_PEDIDOS "/fila-suelle-pedidos-v2"
#define PREFIXO_RESP "/fila-suelle-resp-"

typedef struct { 
    int n1, 
    n2; 
    pid_t pid_a; 
} pedidoT;

typedef struct { 
    int soma; 
} respostaT;

int main(void)
{
    struct mq_attr attr_ped = { 
        .mq_maxmsg = 10, 
        .mq_msgsize = sizeof(pedidoT) 
    };

    mqd_t fila_ped = mq_open(
        FILA_PEDIDOS, O_RDONLY | O_CREAT, 
        0666, 
        &attr_ped);

    if (fila_ped == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    while (1) {
        pedidoT pedido;
        if (mq_receive(fila_ped, (char *)&pedido, sizeof(pedido), NULL) < 0) {
            perror("mq_receive");
            exit(1);
        }

        respostaT resp = { pedido.n1 + pedido.n2 };
        printf("[B %d] pedido de A %d: %d + %d = %d\n",
               getpid(), pedido.pid_a, pedido.n1, pedido.n2, resp.soma);
        fflush(stdout);

        sleep(1);

        // abre a fila de respostas exclusiva do A que pediu
        char nome_resp[64];
        snprintf(nome_resp, sizeof(nome_resp), PREFIXO_RESP "%d", pedido.pid_a);
        mqd_t fila_resp = mq_open(nome_resp, O_WRONLY);
        if (fila_resp == (mqd_t)-1) {
            perror("mq_open (fila do A)");   // o A pode ter sido encerrado
            continue;
        }
        mq_send(fila_resp, (char *)&resp, sizeof(resp), 0);
        mq_close(fila_resp);
    }
    return 0;
}