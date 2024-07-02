#include <stdlib.h>

#ifndef TRABALHO_2_GERA_DADOS_H
#define TRABALHO_2_GERA_DADOS_H

void gera_pal_aleatoria(char *str, size_t size);

void gera_arquivo(const char *filename);

int* valores_alts(const char *nome_arquivo, int qtd_valores_aleatorios);

#endif //TRABALHO_2_GERA_DADOS_H
