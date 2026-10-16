// comum.c - programa principal, igual para as tres versoes.
// Cria duas threads que retiram o primeiro elemento da fila e poem um novo no fim.
//
// Uso:
//   ./programa          -> imprime cada operacao (só encerra com Ctrl+C)
//   ./programa -m 10    -> modo medicao: nao imprime as operacoes e mede,
//                          durante 10 segundos, as insercoes por segundo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "fila.h"
#include "comum.h"

#define NUM_THREADS 2

static fila_t fila;          // estrutura compartilhada pelas threads
static int modo_medicao = 0; // 1 = nao imprime as operacoes

// Contadores por thread. Cada um fica em sua propria linha de cache (64 bytes)
// para que uma thread nao atrapalhe a outra ao incrementar o seu.
typedef struct {
    volatile long insercoes;   // quantas insercoes a thread fez
    volatile long erros;        // quantas vezes encontrou a fila inconsistente
    char preenchimento[48];
} contador_t;

static contador_t contador[NUM_THREADS];

static void *corpo_thread(void *arg)
{
    int id = (int)(long)arg;
    unsigned int semente = time(NULL) ^ (id * 7919);  // semente propria por thread

    while (1) {
        entra(id);  //  inicio da secao critica!

        int velho = -1;
        int novo = rand_r(&semente) % 100;
        int falha_retira = fila_retira(&fila, &velho);
        int falha_poe = fila_poe(&fila, novo);
        int inconsistente = falha_retira || falha_poe || fila.tamanho != CAPACIDADE;

        if (!falha_poe)
            contador[id].insercoes++;   // conta so as insercoes bem-sucedidas
        if (inconsistente)
            contador[id].erros++;

        if (!modo_medicao) {
            printf("thread %d: tira %2d, poe %2d, fila: ", id, velho, novo);
            fila_imprime(&fila);
            if (inconsistente)
                printf("  <-- FILA INCONSISTENTE (tamanho %d)", fila.tamanho);
            printf("\n");
        }

        sai(id);    // ===== fim da secao critica =====
    }
    return NULL;
}

static long total(int campo_erros)
{
    long soma = 0;
    for (int i = 0; i < NUM_THREADS; i++)
        soma += campo_erros ? contador[i].erros : contador[i].insercoes;
    return soma;
}

int main(int argc, char *argv[])
{
    int segundos = 0;
    if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        modo_medicao = 1;
        segundos = atoi(argv[2]);
        if (segundos <= 0) segundos = 10;
    }

    // fila inicialmente cheia, com valores aleatorios
    srandom(time(NULL));
    fila_inicia(&fila);
    for (int i = 0; i < CAPACIDADE; i++)
        fila_poe(&fila, random() % 100);

    pthread_t thread[NUM_THREADS];
    for (long i = 0; i < NUM_THREADS; i++)
        pthread_create(&thread[i], NULL, corpo_thread, (void *)i);

    if (!modo_medicao) {
        // modo normal: as threads rodam para sempre
        for (int i = 0; i < NUM_THREADS; i++)
            pthread_join(thread[i], NULL);
        return 0;
    }

    // modo medicao: a thread principal le os contadores a cada segundo
    printf("Mecanismo: %s\n", NOME_MECANISMO);
    long anterior = 0;
    for (int s = 1; s <= segundos; s++) {
        sleep(1);
        long atual = total(0);
        printf("segundo %2d: %12ld insercoes/s\n", s, atual - anterior);
        anterior = atual;
    }
    long ins = total(0);
    printf("Media: %.0f insercoes/s (t0: %ld, t1: %ld)\n",
           (double)ins / segundos, contador[0].insercoes, contador[1].insercoes);
    printf("Operacoes com fila inconsistente: %ld\n", total(1));
    printf("Tamanho final da fila: %d (deveria ser %d)\n", fila.tamanho, CAPACIDADE);

    // encerra o processo (e as threads também, que estao em laco infinito)
    exit(0);
}
