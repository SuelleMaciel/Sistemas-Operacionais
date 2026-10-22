/*
 * escalonador.c - Simulador de algoritmos de escalonamento de processador
 *
 * Algoritmos simulados:
 * 1. FCFS (First Come, First Served)
 * 2. SJF (Shortest Job First)
 * 3. Por prioridade, sem preempcao
 * 4. Por prioridade, com preempcao por prioridade
 * 5. Round-Robin com quantum = 2s, sem prioridade
 * 6. Round-Robin com prioridade e envelhecimento (tq = 2, alfa = 1)
 *
 * Entrada (stdin): uma linha por tarefa, com tres inteiros separados por espacos:
 *   data_de_criacao  duracao_em_segundos  prioridade_estatica
 * A tarefa da primeira linha e P1, a da segunda e P2, e assim por diante
 * A escala de prioridades e positiva: quanto MAIOR o numero, MAIOR a prioridade
 *
 * Saida (stdout), para cada algoritmo: diagrama de tempo, tempo medio de vida,
 * tempo medio de espera e numero de trocas de contexto.
 *
 * Compilacao: gcc -Wall escalonador.c -o escalonador
 * Uso:        ./Escalonador < entrada.txt        (simula os 6 algoritmos)
 *             ./Escalonador 5 < entrada.txt      (simula apenas o algoritmo 5)
 */
#include <stdio.h>
#include <stdlib.h>

#define MAX_TAREFAS 100
#define QUANTUM     2   // quantum dos algoritmos round-robin, em segundos 
#define ALFA        1   // fator de envelhecimento

// Estados possiveis de uma tarefa
typedef enum { NOVA, PRONTA, RODANDO, TERMINADA } estado_t;

// Dados de cada tarefa (sugestao do enunciado) 
typedef struct {
    int id;
    int criacao;
    int duracao;
    int prio_est;
    int prio_din;
    estado_t estado;
    int inicio;
    int fim;
    int executado;
    int no_quantum;
    int espera;   
} tarefa_t;

// Politicas de escalonamento 
typedef enum { FCFS = 1, SJF, PRIO_SEM_PREEMP, PRIO_COM_PREEMP, RR, RR_PRIO_ENV } politica_t;

static const char *NOME_POLITICA[] = {
    "",
    "FCFS (First Come, First Served)",
    "SJF (Shortest Job First)",
    "Por prioridade, sem preempcao",
    "Por prioridade, com preempcao por prioridade",
    "Round-Robin (quantum = 2s), sem prioridade",
    "Round-Robin com prioridade e envelhecimento (tq = 2, alfa = 1)",
};

// Dados lidos da entrada (preservados entre as simulacoes) 
static int n_tarefas = 0;
static int entrada[MAX_TAREFAS][3];

// Estado da simulacao
static tarefa_t tarefa[MAX_TAREFAS];
static int fila[MAX_TAREFAS];   // fila de prontas: indices das tarefas, em ordem de chegada 
static int tam_fila = 0;

// Fila de prontas
// Coloca a tarefa i no fim da fila de prontas 
static void fila_insere(int i)
{
    fila[tam_fila++] = i;
    tarefa[i].estado = PRONTA;
}

// Retira da fila a tarefa que esta na posicao pos e devolve seu indice
static int fila_retira(int pos)
{
    int i = fila[pos];
    for (int k = pos; k < tam_fila - 1; k++)
        fila[k] = fila[k + 1];
    tam_fila--;
    return i;
}

/* Escolha da proxima tarefa (o que muda de uma politica para outra)
   devolve a POSICAO, na fila de prontas, da tarefa escolhida.
   em caso de empate, vence a que esta ha mais tempo na fila.          
*/
static int escolhe(politica_t pol)
{
    int escolhida = 0;

    switch (pol) {
    case FCFS:
    case RR:
        // a primeira da fila
        escolhida = 0;
        break;

    case SJF:
        // a de menor duracao 
        for (int k = 1; k < tam_fila; k++)
            if (tarefa[fila[k]].duracao < tarefa[fila[escolhida]].duracao)
                escolhida = k;
        break;

    case PRIO_SEM_PREEMP:
    case PRIO_COM_PREEMP:
        // a de maior prioridade estatica
        for (int k = 1; k < tam_fila; k++)
            if (tarefa[fila[k]].prio_est > tarefa[fila[escolhida]].prio_est)
                escolhida = k;
        break;

    case RR_PRIO_ENV:
        // a de maior prioridade dinamica
        for (int k = 1; k < tam_fila; k++)
            if (tarefa[fila[k]].prio_din > tarefa[fila[escolhida]].prio_din)
                escolhida = k;

        /* envelhecimento: a escolhida volta a sua prioridade estatica
           e as demais tarefas prontas tem a prioridade dinamica aumentada */
        for (int k = 0; k < tam_fila; k++) {
            tarefa_t *t = &tarefa[fila[k]];
            if (k == escolhida)
                t->prio_din = t->prio_est;
            else
                t->prio_din += ALFA;
        }
        break;
    }
    return escolhida;
}

// Indica se a politica usa quantum
static int usa_quantum(politica_t pol)
{
    return pol == RR || pol == RR_PRIO_ENV;
}

// Impressao do diagrama
static void imprime_cabecalho(void)
{
    printf("tempo ");
    for (int i = 0; i < n_tarefas; i++)
        printf(" P%-2d", i + 1);
    printf("\n");
}

// "##" = rodando, "--" = pronta; em branco = ainda nao criada ou ja terminada
static void imprime_linha(int t)
{
    printf("%2d-%2d ", t, t + 1);
    for (int i = 0; i < n_tarefas; i++) {
        switch (tarefa[i].estado) {
        case RODANDO: printf("  ##"); break;
        case PRONTA:  printf("  --"); break;
        default:      printf("    "); break;
        }
    }
    printf("\n");
}

// Simulacao (seguindo o pseudocodigo sugerido na atividade)
static void simula(politica_t pol)
{
    // inicializa as tarefas a partir dos dados lidos
    for (int i = 0; i < n_tarefas; i++) {
        tarefa[i] = (tarefa_t){
            .id = i + 1,
            .criacao = entrada[i][0],
            .duracao = entrada[i][1],
            .prio_est = entrada[i][2],
            .prio_din = entrada[i][2],
            .estado = NOVA,
            .inicio = -1,
            .fim = -1,
        };
    }
    tam_fila = 0;

    int rodando = -1;      // indice da tarefa no processador (-1 = livre)
    int anterior = -1;     //ultima tarefa que usou o processador 
    int trocas = 0;        // trocas de contexto
    int terminadas = 0;

    printf("\n=== %d. %s ===\n\n", pol, NOME_POLITICA[pol]);
    imprime_cabecalho();

    int t = 0;
    /* O pseudocodigo usa "enquanto t < tmax". Como o instante final so e
       conhecido ao fim da simulacao, o laco termina quando todas as tarefas
       estiverem concluidas. */
    while (1) {

        // se ha uma tarefa rodando 
        if (rodando >= 0) {
            tarefa_t *tr = &tarefa[rodando];
            if (tr->executado == tr->duracao) {
                // chegou ao fim da execucao: migra para terminada e libera o processador 
                tr->estado = TERMINADA;
                tr->fim = t;
                terminadas++;
                rodando = -1;
            } else if (usa_quantum(pol) && tr->no_quantum == QUANTUM) {
                // chegou ao fim do quantum: volta para a fila de prontas e libera o processador 
                fila_insere(rodando);
                rodando = -1;
            }
        }

        if (terminadas == n_tarefas)
            break;

        // para cada tarefa i: se a tarefa inicia agora, entra na fila de prontas 
        for (int i = 0; i < n_tarefas; i++)
            if (tarefa[i].estado == NOVA && tarefa[i].criacao == t)
                fila_insere(i);

        /* ajuste para a politica preemptiva: se chegou (ou ja esta na fila) uma tarefa
           com prioridade MAIOR que a da tarefa rodando, a tarefa rodando e preemptada */
        if (pol == PRIO_COM_PREEMP && rodando >= 0) {
            for (int k = 0; k < tam_fila; k++) {
                if (tarefa[fila[k]].prio_est > tarefa[rodando].prio_est) {
                    fila_insere(rodando);
                    rodando = -1;
                    break;
                }
            }
        }

        // se o processador estiver livre e houver tarefa na fila de prontas 
        if (rodando < 0 && tam_fila > 0) {
            int i = fila_retira(escolhe(pol));   // escolhe uma tarefa da fila 
            tarefa[i].estado = RODANDO;          // migra para "rodando" 
            tarefa[i].no_quantum = 0;
            if (tarefa[i].inicio < 0)
                tarefa[i].inicio = t;
            if (anterior >= 0 && anterior != i)
                trocas++; // o processador passou para outra tarefa 
            anterior = i;
            rodando = i;
        }

        // imprime a linha do diagrama com o estado de cada tarefa 
        imprime_linha(t);

        // incrementa o tempo e os contadores 
        t++;
        if (rodando >= 0) {
            tarefa[rodando].executado++;
            tarefa[rodando].no_quantum++;
        }
        for (int k = 0; k < tam_fila; k++)
            tarefa[fila[k]].espera++;
    }

    // calcula e imprime os tempos medios 
    double soma_vida = 0, soma_espera = 0;
    for (int i = 0; i < n_tarefas; i++) {
        soma_vida += tarefa[i].fim - tarefa[i].criacao;   //tempo de vida (turnaround) 
        soma_espera += tarefa[i].espera;
    }
    printf("\nTempo medio de vida:   %.2f s\n", soma_vida / n_tarefas);
    printf("Tempo medio de espera: %.2f s\n", soma_espera / n_tarefas);
    printf("Trocas de contexto:    %d\n", trocas);
}

int main(int argc, char *argv[])
{
    // le os dados das tarefas da entrada padrao
    int c, d, p;
    while (scanf("%d %d %d", &c, &d, &p) == 3) {
        if (n_tarefas == MAX_TAREFAS) {
            fprintf(stderr, "Limite de %d tarefas atingido.\n", MAX_TAREFAS);
            break;
        }
        if (c < 0 || d <= 0) {
            fprintf(stderr, "Tarefa ignorada (dados invalidos): %d %d %d\n", c, d, p);
            continue;
        }
        entrada[n_tarefas][0] = c;
        entrada[n_tarefas][1] = d;
        entrada[n_tarefas][2] = p;
        n_tarefas++;
    }

    if (n_tarefas == 0) {
        fprintf(stderr, "Uso: %s [1-6] < arquivo_de_entrada\n", argv[0]);
        return 1;
    }

    if (argc > 1) {
        // simula apenas o algoritmo escolhido 
        int pol = atoi(argv[1]);
        if (pol < FCFS || pol > RR_PRIO_ENV) {
            fprintf(stderr, "Algoritmo invalido: use um numero de 1 a 6.\n");
            return 1;
        }
        simula((politica_t)pol);
    } else {
        // simula todos os algoritmos 
        for (int pol = FCFS; pol <= RR_PRIO_ENV; pol++)
            simula((politica_t)pol);
    }
    return 0;
}