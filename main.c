#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gera_dados.h>

int main() {
    int indice, indiceBusca, opt, linha, ret, ordem, i;
    clock_t begin, end;
    float tempos_arvore[30], tempos_arquivo[30], tempo_arvore, tempo_arquivo, min_arvore, max_arvore, min_arquivo,
    max_arquivo, media_arvore, media_arquivo;
    char nomeArquivo[50] = {"arq1.txt"}, result1[11], result2[11], result3[11];
    b_tree *arv = NULL;

    gera_arquivo("dados.txt");
    do {
        printf("\n\tMENU\n");
        printf("1. Criar Indice\n");
        printf("2. Procurar elementos\n");
        printf("3. Remover registro\n");
        printf("4. Sair\n");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &opt);

        switch (opt) {
            case 1: // Criar índice
                // Exclui árvore que possa estar alocada para gerar outra a partir dos dados fornecidos pelo usuário
                if (arv) {
                    exclui_b_tree(arv, raiz(arv));
                    free(arv);
                }
                arv = NULL;

                // Obtém do usuário, a ordem da árvore
                printf("Digite a ordem da arvore (min 4, max 100): ");
                scanf("%d", &ordem);
                while (ordem < 3 || ordem > 100) {
                    printf("Valor de ordem invalido.\n");
                    printf("Digite a ordem da arvore (min 4, max 100): ");
                    scanf("%d", &ordem);
                }

                // Cria a B Tree com a ordem informada pelo usuário
                arv = cria_b_tree(ordem);
                // Obtém o nome do arquivo que contém os dados que irão gerar a árvore
                printf("Digite o nome do arquivo: ");
                scanf(" %s", nomeArquivo);
                // Variável para validar o processamento do arquivo
                ret = processa_arquivo(arv, nomeArquivo);
                // Se houve sucesso no processamento do arquivo
                if (ret) {
                    printf("\nArvore:\n");
                    // Imprime os nós da árvore com índice, quantidade de chaves e se é folha ou não
                    imprime_b_tree(raiz(arv));
                    // E a quantidade de nós
                    printf("Quantidade de nos: %d\n\n", nro_nos(arv));
                } else {
                    // Se não, informa ao usuário a falha na leitura
                    printf("Erro ao ao carregar arquivo\n\n");
                }
                break;

            case 2: // Procurar elementos
                // Informa ao usuário caso a árvore seja nula
                if (arv == NULL) {
                    printf("A arvore nao foi criada\n\n");
                    break;
                }

                // Carrega o arquivo no modo leitura
                FILE *arq;
                arq = fopen(nomeArquivo, "r");

                // Informa ao usuário caso não tenha sido possível carregar o arquivo
                if (!arq) {
                    printf("Falha ao carregar o arquivo\n\n");
                    break;
                }

                // Gera arquivo com 30 elementos aleatórios do arquivo de dados
                selecionar30Elementos(nomeArquivo, "30elem.txt");

                // Carrega o arquivo com 30 elementos aleatórios no modo leitura
                FILE *busca30 = fopen("30elem.txt", "r");
                if (!busca30) {
                    printf("Falha ao carregar o arquivo com elementos aleatorios\n\n");
                    fclose(arq);
                    break;
                }

            // Busca na árvore. Obtém a linha referente ao índice buscado e acessa os dados deste índice diretamente no arquivo
                i = 0;
                while (fscanf(busca30, "%d", &indice) == 1) {
                    printf("\nBusca na arvore:");

                    // Inicia a contagem do tempo
                    begin = clock();

                    // Obtém a linha referente ao dado procurado
                    linha = procura_b_tree(arv, indice, raiz(arv));

                    if (linha != -1) {
                        printf("\nIndice procurado: %d\nLinha: %d\n", indice, linha);
                        // 40 é o número de bytes na linha. (linha - 1) porque os arrays em C são indexados a partir de 0
                        // e o +6 é para ignorar os bytes do índice e pegar apenas as informações dos dados
                        fseek(arq, 40 * (linha - 1) + 6, SEEK_SET);
                        fscanf(arq, " %[^\t] %[^\t] %[^\n]", result1, result2, result3);
                        printf("Valores: %s %s %s\n", result1, result2, result3);
                    } else {
                        printf("Elemento nao encontrado na arvore\n");
                    }

                    // Finaliza a contagem de tempo
                    end = clock();
                    // Calcula o tempo gasto na procura na árvore
                    tempo_arvore = (float) (end - begin) / CLOCKS_PER_SEC;
                    printf("Tempo: %.10f segundos\n", tempo_arvore);
                    // Armazena o tempo no vetor de tempos, usado para cálculo da média
                    tempos_arvore[i] = tempo_arvore;

                    // Busca no arquivo. Percorre linha por linha buscando o índice desejado
                    printf("\nBusca no arquivo:");
                    rewind(arq);

                    // Inicia a contagem de tempo
                    begin = clock();

                    int encontrou = 0;
                    while (!feof(arq)) {
                        fscanf(arq, " %d", &indiceBusca);
                        if (indiceBusca == indice) {
                            printf("\nIndice procurado: %d\nLinha: %d\n", indice, linha);
                            fscanf(arq, " %[^\t] %[^\t] %[^\n]", result1, result2, result3);
                            printf("Valores: %s %s %s\n", result1, result2, result3);
                            encontrou = 1;
                            break;
                        } else {
                            fseek(arq, 33, 1);
                        }
                        fgetc(arq);
                    }
                    if (!encontrou) {
                        printf("Elemento nao encontrado no arquivo\n");
                    }

                    // Finaliza a contagem de tempo
                    end = clock();
                    // Calcula o tempo gasto na procura no arquivo
                    tempo_arquivo = (float)(end - begin) / CLOCKS_PER_SEC;
                    printf("Tempo: %.10f segundos\n", tempo_arquivo);
                    // Armazena o tempo no vetor de tempos, usado para cálculo da média
                    tempos_arquivo[i] = tempo_arquivo;
                    i++;
                }

                // Fecha os arquivos
                fclose(busca30);
                fclose(arq);


                // Cálculo das métricas de tempo mínimo, máximo e médio
                // Inicialização das variáveis
                min_arvore = tempos_arvore[0];
                max_arvore = tempos_arvore[0];
                min_arquivo = tempos_arquivo[0];
                max_arquivo = tempos_arquivo[0];
                media_arvore = 0;
                media_arquivo = 0;

                for (i = 0; i < 30; i++) {
                    media_arvore += tempos_arvore[i];
                    media_arquivo += tempos_arquivo[i];

                    if (tempos_arvore[i] < min_arvore) {
                        min_arvore = tempos_arvore[i];
                    }
                    if (tempos_arvore[i] > max_arvore) {
                        max_arvore = tempos_arvore[i];
                    }

                    if (tempos_arquivo[i] < min_arquivo) {
                        min_arquivo = tempos_arquivo[i];
                    }
                    if (tempos_arquivo[i] > max_arquivo) {
                        max_arquivo = tempos_arquivo[i];
                    }
                }

                // Calcula as médias
                media_arvore /= 30;
                media_arquivo /= 30;

                // Apresenta os resultados
                printf("\nMetricas de tempo para busca na arvore:\n");
                printf("Tempo minimo: %.10f segundos\n", min_arvore);
                printf("Tempo maximo: %.10f segundos\n", max_arvore);
                printf("Tempo medio: %.10f segundos\n", media_arvore);

                printf("\nMetricas de tempo para busca no arquivo:\n");
                printf("Tempo minimo: %.10f segundos\n", min_arquivo);
                printf("Tempo maximo: %.10f segundos\n", max_arquivo);
                printf("Tempo medio: %.10f segundos\n", media_arquivo);

                break;

            case 3: // Remover registro
                // Informa ao usuário caso a árvore seja nula
                if (!arv) {
                    printf("A arvore nao foi criada\n\n");
                    break;
                }

                // Obtém do usuário, o índice do elemento a ser removido
                printf("Digite o indice do elemento a ser removido: ");
                scanf("%d", &indice);
                // Variável para validar a remoção do elemento
                ret = remove_b_tree(arv, indice);
                if (ret)
                    printf("Elemento removido com sucesso\n");
                else
                    printf("Falha ao remover elemento\n");

                // Imprime a árvore após a remoção do elemento, bem como a quantidade de nós restante
                printf("\nArvore:\n");
                imprime_b_tree(raiz(arv));
                printf("Quantidade de nos: %d\n\n", nro_nos(arv));

                break;

            case 4: // Sair
                printf("Saindo...\n\n");
                break;

            default: // Validação da escolha do menu
                printf("Valor invalido\n\n");
                break;
        }
    } while (opt != 4);

    if (!arv) {
        return 0;
    }

    // Libera a memória alocada
    exclui_b_tree(arv, raiz(arv));
    free(arv);
    return 0;
}
