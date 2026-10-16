// alternancia.c - item 2: alternancia estrita
// As threads entram na secao critica sempre na ordem t0, t1, t0, t1...
#include <stdatomic.h>
#include "comum.h"

const char *NOME_MECANISMO = "alternancia estrita";

// De quem e a vez. Atomica para que cada thread enxergue
// imediatamente a alteracao feita pela outra.
static atomic_int vez = 0;

void entra(int id)
{
    while (atomic_load(&vez) != id)
        ;  // espera ocupada: fica testando ate chegar a sua vez
}

void sai(int id)
{
    atomic_store(&vez, 1 - id);  // passa a vez para a outra thread
}
