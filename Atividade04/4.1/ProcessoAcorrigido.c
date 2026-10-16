// Correção: cada A cria sua propria fila de respostas e informa o PID no pedido, para que B saiba pra quem responder
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

#define FILA_PEDIDOS "/fila-suelle-pedidos-v2"   // compartilhada: todos os A -> todos os B
#define PREFIXO_RESP "/fila-suelle-resp-"        // uma por A: /fila-suelle-resp-<pid>

typedef struct { int n1, n2; pid_t pid_a; } pedidoT;
typedef struct { int soma; } respostaT;

static char nome_resp[64];

static void encerra(int sig)
{
    (void)sig;
    mq_unlink(nome_resp);   // remove a fila particular ao sair (Ctrl+C)
    _exit(0);
}

int main(void)
{
    struct mq_attr attr_ped  = { 
        .mq_maxmsg = 10, 
        .mq_msgsize = sizeof(pedidoT) 
    };
    struct mq_attr attr_resp = { 
        .mq_maxmsg = 10, 
        .mq_msgsize = sizeof(respostaT) 
    };

    snprintf(nome_resp, sizeof(nome_resp), PREFIXO_RESP "%d", getpid());

    mqd_t fila_ped  = mq_open(FILA_PEDIDOS, O_WRONLY | O_CREAT, 0666, &attr_ped);
    mqd_t fila_resp = mq_open(nome_resp,    O_RDONLY | O_CREAT, 0666, &attr_resp);
    if (fila_ped == (mqd_t)-1 || fila_resp == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    signal(SIGINT, encerra);
    signal(SIGTERM, encerra);
    srandom(time(NULL) ^ getpid());

    while (1) {
        pedidoT pedido = { random() % 100, random() % 100, getpid() };

        if (mq_send(fila_ped, (char *)&pedido, sizeof(pedido), 0) < 0) {
            perror("mq_send");
            break;
        }

        respostaT resp;
        if (mq_receive(fila_resp, (char *)&resp, sizeof(resp), NULL) < 0) {
            perror("mq_receive");
            break;
        }

        printf("[A %d] %d + %d = %d%s\n", getpid(), pedido.n1, pedido.n2, resp.soma,
               (pedido.n1 + pedido.n2 == resp.soma) ? "" : "   <-- ERRADO!");
        fflush(stdout);
    }

    mq_unlink(nome_resp);
    return 0;
}