// peterson.c - item 3: algoritmo de Peterson para duas threads
#include <stdatomic.h>
#include "comum.h"

const char *NOME_MECANISMO = "algoritmo de Peterson";

// As variaveis sao atomicas (ordem sequencial) por um motivo importante:
// processadores modernos podem reordenar uma escrita seguida de uma leitura.
// Com variaveis comuns, as duas threads poderiam entrar juntas na secao critica.
static atomic_int quer[2] = { 0, 0 };  // quer[i] = 1 se a thread i quer entrar
static atomic_int vez = 0;             // quem cede a vez em caso de disputa

void entra(int id)
{
    int outro = 1 - id;
    atomic_store(&quer[id], 1);   // indica que quer entrar
    atomic_store(&vez, outro);    // cede a vez para a outra
    // espera enquanto a outra quiser entrar E for a vez dela
    while (atomic_load(&quer[outro]) && atomic_load(&vez) == outro);
}

void sai(int id)
{
    atomic_store(&quer[id], 0);   // deixa de querer entrar
}
