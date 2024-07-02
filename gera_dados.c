#include "gera_dados.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_LINES 15000
#define MIN_NUMBER 10000
#define MAX_NUMBER 99999
#define STRING_SIZE 10

void gera_pal_aleatoria(char *str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int)(sizeof(charset) - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
}

void gera_arquivo(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }

    // Vetor para alocar números únicos
    int *numbers = malloc(NUM_LINES * sizeof(int));
    if (numbers == NULL) {
        perror("Erro ao alocar memória");
        exit(EXIT_FAILURE);
    }

    // Inicializa o gerador de número aleatório
    srand((unsigned int)time(NULL));

    // Gera números aleatórios únicos
    for (int i = 0; i < NUM_LINES; i++) {
        int num;
        int is_unique;
        do {
            is_unique = 1;
            num = MIN_NUMBER + rand() % (MAX_NUMBER - MIN_NUMBER + 1);
            // Confere se o número é único
            for (int j = 0; j < i; j++) {
                if (numbers[j] == num) {
                    is_unique = 0;
                    break;
                }
            }
        } while (!is_unique);
        numbers[i] = num;
    }

    // Escreve no arquivo
    for (int i = 0; i < NUM_LINES; i++) {
        char str1[STRING_SIZE + 1];
        char str2[STRING_SIZE + 1];
        char str3[STRING_SIZE + 1];
        gera_pal_aleatoria(str1, STRING_SIZE + 1);
        gera_pal_aleatoria(str2, STRING_SIZE + 1);
        gera_pal_aleatoria(str3, STRING_SIZE + 1);

        fprintf(file, "%d\t%s\t%s\t%s\n", numbers[i], str1, str2, str3);
    }

    // Libera memória
    free(numbers);
    fclose(file);
}
