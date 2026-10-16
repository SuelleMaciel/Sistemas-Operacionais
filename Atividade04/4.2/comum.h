// comum.h - interface entre o programa principal (comum.c) e cada
// mecanismo de sincronizacao (sem_sincronizacao.c, alternancia.c, peterson.c)
#ifndef COMUM_H
#define COMUM_H

void entra(int id);   // entrada na secao critica
void sai(int id);     // saida da secao critica

// Nome do mecanismo exibido na saida
extern const char *NOME_MECANISMO;

#endif
