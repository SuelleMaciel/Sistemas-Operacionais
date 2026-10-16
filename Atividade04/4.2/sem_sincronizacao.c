// sem_sincronizacao.c - item 1: nenhuma coordenacao entre as threads
#include "comum.h"

const char *NOME_MECANISMO = "sem sincronizacao";

void entra(int id) { (void)id; }  // nao faz nada, as threads entram livremente
void sai(int id)   { (void)id; }
