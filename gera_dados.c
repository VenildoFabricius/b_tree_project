#include "gera_dados.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_PALAVRAS 10
#define NUM_LINHAS 11000

// Função para gerar uma palavra aleatória de um dado comprimento
void gerarPalAleatoria(char *pal, int tamanho) {
    // Conjunto de caracteres possíveis (letras maiúsculas e minúsculas)
    static const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < tamanho; i++) {
        // Gera cada caractere da palavra aleatória
        int key = rand() % (int)(sizeof(caracteres) - 1);
        pal[i] = caracteres[key];
    }
    pal[tamanho] = '\0';
}

int gera_dados(){
    FILE *file;
    file = fopen("dados.txt", "w");
    if (!file) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < NUM_LINHAS; i++) {
        int indice = rand() % 90000 + 10000; // Gera um número aleatório entre 10000 e 99999
        char pal1[TAM_PALAVRAS + 1];
        char pal2[TAM_PALAVRAS + 1];
        char pal3[TAM_PALAVRAS + 1];

        gerarPalAleatoria(pal1, TAM_PALAVRAS);
        gerarPalAleatoria(pal2, TAM_PALAVRAS);
        gerarPalAleatoria(pal3, TAM_PALAVRAS);

        fprintf(file, "%d\t%s\t%s\t%s\n", indice, pal1, pal2, pal3);
    }
    fclose(file);
    return 0;
}

