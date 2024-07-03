#include "btree.h"
#include "gera_dados.h"
#include "fcs_main.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    int opt;
    char nomeArquivo[20];
    b_tree *arv = NULL;

    do {
        opt = menu();

        if (opt == 1) {
            // Obtém o nome do arquivo que contém os dados que irão gerar a árvore
            printf("Digite o nome do arquivo de origem dos dados: ");
            scanf(" %s", nomeArquivo);
            // Gera o arquivo que contém os dados que irão gerar a árvore
            gera_arquivo(nomeArquivo);
        }

        switch (opt) {
            case 1: // Criar índice
                arv = case_1(arv, nomeArquivo);
                break;

            case 2: // Procurar elementos
                case_2(arv, nomeArquivo);
                break;

            case 3: // Remover registro
                case_3(arv);
                break;

            case 4: // Sair
                printf("Saindo...\n\n");
                break;

            default: // Validação da escolha do menu
                printf("Valor invalido\n\n");
                break;
        }
    } while (opt != 4);

    // Libera a memória alocada
    exclui_b_tree(arv, raiz(arv));
    free(arv);
    return 0;
}
