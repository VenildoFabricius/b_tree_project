#include <stdlib.h>

#ifndef TRABALHO_2_GERA_DADOS_H
#define TRABALHO_2_GERA_DADOS_H

void gera_pal_aleatoria(char *str, size_t size);

void gera_arquivo(const char *filename);

void shuffle(int *array, int n);

void selecionar30Elementos(char *arquivoEntrada, char *arquivoSaida);

#endif //TRABALHO_2_GERA_DADOS_H
