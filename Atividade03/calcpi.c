#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define NUM_TERMOS 1000000000LL

long long num_termos = NUM_TERMOS;
int       num_threads;
double   *result;

void *calcula_parcial(void *arg)
{
    long id = (long) arg;

    long long total = num_termos;
    int       nt    = num_threads;

    long long bloco  = total / nt;
    long long inicio = id * bloco;
    long long fim    = (id == nt - 1) ? total : inicio + bloco;

    double soma  = 0.0;
    double sinal = (inicio % 2 == 0) ? 1.0 : -1.0;

    for (long long n = inicio; n < fim; n++) {
        soma  += sinal / (2.0 * n + 1.0);
        sinal  = -sinal;
    }

    result[id] = soma;
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <num_threads>\n", argv[0]);
        return 1;
    }

    num_threads = atoi(argv[1]);
    if (num_threads < 1) {
        fprintf(stderr, "Número de threads deve ser >= 1\n");
        return 1;
    }

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    result = malloc(num_threads * sizeof(double));
    if (!threads || !result) {
        fprintf(stderr, "Erro de alocação\n");
        return 1;
    }

    for (long i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, calcula_parcial, (void *) i) != 0) {
            fprintf(stderr, "Erro ao criar thread %ld\n", i);
            return 1;
        }
    }

    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    double soma = 0.0;
    for (int i = 0; i < num_threads; i++)
        soma += result[i];

    double pi = 4.0 * soma;
    printf("Threads: %d\n", num_threads);
    printf("Pi calculado: %.15f\n", pi);
    printf("Erro:         %.15e\n", fabs(pi - M_PI));

    free(threads);
    free(result);
    return 0;
}
