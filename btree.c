#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

b_tree *cria_b_tree(int ordem){
    // Aloca a árvore
    b_tree *arv = (b_tree*) malloc(sizeof(b_tree));
    // Atribui a ordem passada como parâmetro
    arv->ordem = ordem;
    // Aloca a sentinela
    arv->sentinela = (no*) malloc(sizeof(no));
    // Aloca o keyor do filho da sentinela, com tamanho único
    arv->sentinela->filhos = (no**) malloc(sizeof(no*));
    // Como a árvore é inicialmente vazia, sentinela não possui filho
    arv->sentinela->filhos[0] = NULL;

    // Atribui -1 à quantidade de chaves, indicando que é a sentinela
    arv->sentinela->n = -1;

    return arv;
}

int insere_b_tree(b_tree *arv, int indice, int valor) {
    no *raiz, *atual;
    int achou, i;

    // Se a árvore não foi alocada, não é possível fazer a inserção
    if (!arv)
        return 0;

    // Inserindo em uma árvore vazia
    if (!arv->sentinela->filhos[0]) {
        // Aloca o nó
        raiz = (no *)malloc(sizeof(no));
        // Atribui ao primeiro filho da sentinela
        arv->sentinela->filhos[0] = raiz;
        // Incrementa a quantidade de nós
        arv->quantidadeNos++;

        // Aloca as chaves do nó, com ordem chaves. A posição extra permite alocar uma chave além da permitida e
        // realizar a operação de split posteriormente
        raiz->key = (chave *)malloc(arv->ordem * sizeof(chave));
        // Aloca o keyor de filhos, com tamanho ordem + 1. Mesmo caso da quantidade de chaves.
        raiz->filhos = (no **)malloc((arv->ordem + 1) * sizeof(no *));
        // Para cada posição do keyor de filhos, atribui NULL, pois a raiz ainda não nenhum filho
        for (i = 0; i <= arv->ordem; i++) {
            raiz->filhos[i] = NULL;
        }

        // Preenche os dados da raiz
        raiz->key[0].indice = indice;
        raiz->key[0].valor = valor;
        raiz->pai = arv->sentinela;
        raiz->folha = 1;
        raiz->n = 1;

        return 1;
    }

    // Caso já possua nós na árvore, um ponteiro auxiliar aponta para a raiz e percorre a árvore
    atual = arv->sentinela->filhos[0];

    // Procura nó folha para inserir elemento
    while (atual->folha != 1) {
        // Variável para informar se o nó onde será feita a inserção foi encontrado
        achou = 0;
        // Busca pelo nó onde deve ser feita a inserção
        for (i = 0; i < atual->n; i++) {
            // Compara as chaves do nó atual até encontrar uma chave com índice maior do que o índice do nó a ser inserido
            if (indice < atual->key[i].indice) {
                // Encontrou o keyor de filhos onde a inserção deve ser feita
                atual = atual->filhos[i];
                achou = 1;
                // Sai do loop
                break;
            }
        }
        // Se o índice da chave a ser inserida for maior que o maior índice do nó atual, a chave pertence a um filho
        // direito do nó atual e o loop anterior não pôde encontrar
        if (!achou) {
            // A chave deve ser inserida no keyor com chaves maiores que o maior índice do nó atual
            atual = atual->filhos[atual->n];
        }
    }

    // Encontrado o nó, insere-se a chave de forma ordenada, comparando o índice da chave a ser inserida com cada índice
    // do nó atual. A busca começa do maior índice e é decrementada até encontrar a posição de inserção
    for (i = atual->n; i > 0; i--) {
        // Se o índice da chave a ser inserida for maior que o maior índice do nó atual, o ponto de inserção foi
        // encontrado, encerra-se o loop
        if (indice > atual->key[i - 1].indice) {
            break;
        }
        // Se não, os índices são copiados uma posição para a frente do keyor até que a posição de inserção seja encontrada
        atual->key[i] = atual->key[i - 1];
    }
    // Encontrada a posição de inserção, atribui-se o índice e o valor passados como parâmetros à respectiva chave
    atual->key[i].indice = indice;
    atual->key[i].valor = valor;
    // Incrementa a quantidade de chaves do nó
    atual->n++;

    // Se a quantidade de chaves do nó atingiu o limite máximo (ordem), é necessário fazer o balanceamento
    if (atual->n == arv->ordem) {
        bal_insercao(arv, atual);
    }

    return 1;
}

void bal_insercao(b_tree *arv, no *atual) {
    int i, j;
    no *pai;

    // Verifica se o nó atual é a raiz
    if (arv->sentinela->filhos[0] == atual) {
        // Se sim, é necessário criar um novo nó, que será o pai do nó desbalanceado
        pai = (no *)malloc(sizeof(no));
        pai->key = (chave *)malloc(arv->ordem * sizeof(chave));
        pai->filhos = (no **)malloc((arv->ordem + 1) * sizeof(no *));
        for (i = 0; i <= arv->ordem; i++) {
            pai->filhos[i] = NULL;
        }

        // Cria irmão do nó desbalanceado, que será usado na operação de split
        no *irmao = (no *)malloc(sizeof(no));
        irmao->key = (chave *)malloc(arv->ordem * sizeof(chave));
        irmao->filhos = (no **)malloc((arv->ordem + 1) * sizeof(no *));
        for (i = 0; i <= arv->ordem; i++) {
            irmao->filhos[i] = NULL;
        }

        // Atualiza os valores do pai
        // Nó pai não será uma folha
        pai->folha = 0;
        // A quantidade de nós do pai será 1, uma vez que o nó atual era raiz
        pai->n = 1;
        // Como o pai passa a ser a raiz, o pai do pai é a sentinela
        pai->pai = arv->sentinela;
        // E o filho da sentinela passa a ser o pai
        arv->sentinela->filhos[0] = pai;
        // O filho esquerdo do nó pai será o nó atual
        pai->filhos[0] = atual;
        // E o filho direito, o nó irmão criado na operação de split
        pai->filhos[1] = irmao;

        // Encontra o valor central do nó atual e o copia para a raiz
        pai->key[0] = atual->key[(arv->ordem - 1) / 2];

        // Atualiza os valores do irmão
        // Irmão está na mesma altura do nó atual, portanto tem a mesma informação de folha
        irmao->folha = atual->folha;
        // A quantidade de chaves do irmão criado será a metade da ordem da árvore, uma vez que o nó atual foi divido ao meio
        irmao->n = arv->ordem / 2;
        // O pai do irmão será o nó pai criado na operação de split
        irmao->pai = pai;
        // O irmão passa a ser o filho "1" do pai
        irmao->pai_posicao = 1;

        // Insere elementos do nó atual no nó irmão criado
        // Variável para percorrer o nó irmão
        j = 0;
        // Loop começa no meio do nó atual e termina ao atingir ordem-1
        for (i = ((arv->ordem - 1) / 2) + 1; i < arv->ordem; i++) {
            // A cópia do nó atual para o irmão começa na posição posterior à central
            irmao->key[j] = atual->key[i];
            // O mesmo vale para os filhos dos nós
            irmao->filhos[j] = atual->filhos[i];
            // Se o nó atual possuir filhos, estes filhos passam a ser filhos do nó irmão
            if (atual->filhos[i]) {
                atual->filhos[i]->pai = irmao;
                atual->filhos[i]->pai_posicao = j;
                atual->filhos[i] = NULL;
            }
            j++;
        }
        // Copia o último ponteiro do nó atual para o irmão criado
        irmao->filhos[j] = atual->filhos[i];
        if (atual->filhos[i]) {
            atual->filhos[i]->pai = irmao;
            atual->filhos[i]->pai_posicao = j;
            atual->filhos[i] = NULL;
        }

        // Atualiza os valores do nó atual
        // A quantidade de chaves no nó atual passa a ser a metade da ordem-1
        atual->n = (arv->ordem - 1) / 2;
        // Passa a ter como pai o nó criado na operação de split
        atual->pai = pai;
        // Nó atual é o filho "0" do pai
        atual->pai_posicao = 0;

        // Incrementa a quantidade de nós da árvore em 2 (irmão e pai)
        arv->quantidadeNos += 2;

    // Se o nó desbalanceado não for raiz
    } else {
        pai = atual->pai;
        for (i = pai->n; i > atual->pai_posicao; i--) {
            // Os índices são copiados uma posição para a frente do keyor a cada iteração
            pai->key[i] = pai->key[i - 1];
            // A partir da posição atual do nó, os keyores dos filhos do pai são copiados uma posição para a frente
            pai->filhos[i + 1] = pai->filhos[i];
            // E a sua posição em relação ao pai é incrementada
            pai->filhos[i + 1]->pai_posicao++;
        }

        // A posição central do nó atual é copiada para o pai
        pai->key[atual->pai_posicao] = atual->key[(arv->ordem - 1) / 2];

        // Cria-se um irmão para a operação de split
        no *irmao = (no *)malloc(sizeof(no));
        irmao->key = (chave *)malloc(arv->ordem * sizeof(chave));
        irmao->filhos = (no **)malloc((arv->ordem + 1) * sizeof(no *));
        for (i = 0; i <= arv->ordem; i++) {
            irmao->filhos[i] = NULL;
        }

        // O ponteiro de filhos do pai, posterior à posição do nó atual, aponta para o irmão criado
        pai->filhos[atual->pai_posicao + 1] = irmao;

        // Atualiza os valores do nó criado
        // Irmão está na mesma altura do nó atual, portanto tem a mesma informação de folha
        irmao->folha = atual->folha;
        // A quantidade de chaves do irmão criado será a metade da ordem da árvore, uma vez que o nó atual foi divido ao meio
        irmao->n = arv->ordem / 2;
        // O pai do irmão será o mesmo pai do nó atual
        irmao->pai = pai;
        // O irmão passa a ser o filho posterior ao atual
        irmao->pai_posicao = atual->pai_posicao + 1;

        j = 0;
        for (i = ((arv->ordem - 1) / 2) + 1; i < arv->ordem; i++) {
            // A cópia do nó atual para o irmão começa na posição posterior à central
            irmao->key[j] = atual->key[i];
            // O mesmo vale para os filhos dos nós
            irmao->filhos[j] = atual->filhos[i];
            // Se o nó atual possuir filhos, estes filhos passam a ser filhos do nó irmão
            if (irmao->filhos[j]) {
                irmao->filhos[j]->pai = irmao;
                irmao->filhos[j]->pai_posicao = j;
                atual->filhos[i] = NULL;
            }
            j++;
        }
        // Copia o último ponteiro do nó atual para o irmão criado
        irmao->filhos[j] = atual->filhos[i];
        if (atual->filhos[i]) {
            atual->filhos[i]->pai = irmao;
            atual->filhos[i]->pai_posicao = j;
            atual->filhos[i] = NULL;
        }

        // A quantidade de chaves no nó atual passa a ser a metade da ordem-1
        atual->n = (arv->ordem - 1) / 2;

        // Incrementa a quantidade de filhos do pai
        pai->n++;

        // Se a quantidade de chaves do pai atingiu o limite máximo (ordem), é necessário fazer o balanceamento do pai
        if (pai->n == arv->ordem) {
            bal_insercao(arv, pai);

            // Atualiza nó pai caso ele tenha mudado pela chamada da função
//            pai = atual->pai;
        }

        // Incrementa a quantidade de nós da árvore
        arv->quantidadeNos++;
    }
}

int remove_b_tree(b_tree *arv, int indice) {
    if (!arv|| !arv->sentinela->filhos[0])
        return 0;

    int pos = -1, i;
    no *atual = arv->sentinela->filhos[0];

    // Procura indice na B Tree
    do {
        for (i = 0; i < atual->n; i++) {
            if (indice == atual->key[i].indice) {
                // pos armazena posição do elemento no nó
                pos = i;
                break;
            }

            if (indice < atual->key[i].indice) {
                // Caso não ache no vetor, nó atual recebe um de seus filhos
                atual = atual->filhos[i];
                i = -1;
                break;
            }
        }

        // Caso indice seja maior que todos os elementos do vetor, atual recebe último
        // ponteiro do nó
        if (atual != NULL && i == atual->n)
            atual = atual->filhos[i];
    } while (pos == -1 && atual != NULL);

    // Caso não tenha encontrado indice na árvore
    if (atual == NULL)
        return 0;

    if (atual->folha) { // Caso indice a ser removido esteja em um nó folha
        // Copia todos os valores uma posição para frente
        for (i = pos; i < atual->n - 1; i++) {
            atual->key[i] = atual->key[i + 1];
        }
        atual->n--;

        // Verifica se houve desbalanceamento
        if (atual->n < (arv->ordem - 1) / 2 &&
            arv->sentinela->filhos[0] != atual) {
            bal_remocao(arv, atual);
        } else if (atual->n ==
                   0) { // Significa que não existe mais elementos na árvore
            // Libera estruturas do nó atual
            free(atual->key);
            free(atual->filhos);
            free(atual);

            arv->sentinela->filhos[0] = NULL;
            arv->quantidadeNos--;
        }
    } else { // Caso indice esteja em um nó interno da árvore
        // Procura sucessor do valor
        no *filho = atual->filhos[pos + 1];
        while (filho->filhos[0])
            filho = filho->filhos[0];

        // Copia valor do sucessor para lugar do indice removido
        atual->key[pos] = filho->key[0];

        // Copia valores uma posição para frente no nó filho
        for (i = 0; i < filho->n - 1; i++) {
            filho->key[i] = filho->key[i + 1];
        }
        filho->n--;

        // Verifica se houve desbalanceamento no nó filho
        if (filho->n < (arv->ordem - 1) / 2) {
            bal_remocao(arv, filho);
        }
    }

    return 1;
}

void bal_remocao(b_tree *arv, no *atual) {
    int esq = 0, dir = 0;
    no *irmao;
    no *pai = atual->pai;

    // Verifica se é possível fazer rotação com irmão
    if (atual->pai_posicao > 0 && pai->filhos[atual->pai_posicao - 1] != NULL &&
        pai->filhos[atual->pai_posicao - 1]->n >
        (arv->ordem - 1) / 2) { // Indica que é possível fazer rotação esq
        esq = 1;
        irmao = pai->filhos[atual->pai_posicao - 1];
    } else if (pai->n >= atual->pai_posicao + 1 &&
               pai->filhos[atual->pai_posicao + 1]->n >
               (arv->ordem - 1) /
               2) { // Indica que é possível fazer rotação dir
        dir = 1;
        irmao = pai->filhos[atual->pai_posicao + 1];
    }

    if (esq) { // Rotação com irmão da esquerda
        // Copia valores e filhos do nó atual uma posição para trás
        for (int i = atual->n; i > 0; i--) {
            atual->key[i] = atual->key[i - 1];
            atual->filhos[i + 1] = atual->filhos[i];
            if (atual->filhos[i + 1] != NULL)
                atual->filhos[i + 1]->pai_posicao++;
        }
        atual->filhos[1] = atual->filhos[0];
        if (atual->filhos[1] != NULL)
            atual->filhos[1]->pai_posicao++;

        // Copia valor do nó pai para a primeira posição do nó atual
        atual->key[0] = pai->key[atual->pai_posicao - 1];

        // Copia valor da última posição do irmão para a posição do pai que foi
        // copiada para nó atual
        pai->key[atual->pai_posicao - 1] = irmao->key[irmao->n - 1];

        // Muda pai do nó que estava na última posição do vetor do irmão
        atual->filhos[0] = irmao->filhos[irmao->n];
        if (atual->filhos[0] != NULL) {
            atual->filhos[0]->pai = atual;
            atual->filhos[0]->pai_posicao = 0;
        }
        atual->n++;
        irmao->n--;
    } else if (dir) { // Rotação com irmão da direita
        // Copia valor do pai para última posição do vetor do nó atual
        atual->key[atual->n] = pai->key[atual->pai_posicao];

        // Copia valor da primeira posição do irmão para o pai
        pai->key[atual->pai_posicao] = irmao->key[0];

        // Copia primeiro ponteiro do irmão para última posição do nó atual
        atual->filhos[atual->n + 1] = irmao->filhos[0];
        if (atual->filhos[atual->n + 1] != NULL) {
            atual->filhos[atual->n + 1]->pai = atual;
            atual->filhos[atual->n + 1]->pai_posicao = atual->n + 1;
        }

        // Move elementos do nó irmao uma posição para frente
        for (int i = 0; i < irmao->n - 1; i++) {
            irmao->key[i] = irmao->key[i + 1];
            irmao->filhos[i] = irmao->filhos[i + 1];
            if (irmao->filhos[i] != NULL)
                irmao->filhos[i]->pai_posicao--;
        }
        irmao->filhos[irmao->n - 1] = irmao->filhos[irmao->n];
        if (irmao->filhos[irmao->n - 1] != NULL)
            irmao->filhos[irmao->n - 1]->pai_posicao--;

        atual->n++;
        irmao->n--;
    } else {                        // Operação de Merge
        if (atual->pai_posicao < pai->n) { // Merge com irmão da direita
            irmao = pai->filhos[atual->pai_posicao + 1];

            // Copia valor do pai para última posição do nó atual
            atual->key[atual->n] = pai->key[atual->pai_posicao];

            // Copia valores do pai uma posição para frente a partir da posição que
            // foi para nó atual
            for (int i = atual->pai_posicao + 1; i < pai->n; i++) {
                pai->key[i - 1] = pai->key[i];
                pai->filhos[i] = pai->filhos[i + 1];
                if (pai->filhos[i] != NULL)
                    pai->filhos[i]->pai_posicao--;
            }

            // Copia valores do irmão para o nó atual
            int j = atual->n + 1;
            for (int i = 0; i < irmao->n; i++) {
                atual->key[j] = irmao->key[i];
                atual->filhos[j] = irmao->filhos[i];
                if (atual->filhos[j] != NULL) {
                    atual->filhos[j]->pai = atual;
                    atual->filhos[j]->pai_posicao = j;
                }
                j++;
            }

            // Copia último ponteiro do irmão para o nó atual
            atual->filhos[j] = irmao->filhos[irmao->n];
            if (atual->filhos[j] != NULL) {
                atual->filhos[j]->pai = atual;
                atual->filhos[j]->pai_posicao = j;
            }

            // Atual recebeu todos os nós de seu irmão e um de seu pai
            atual->n += irmao->n + 1;

            // Libera estruturas do irmão
            free(irmao->key);
            free(irmao->filhos);
            free(irmao);

            arv->quantidadeNos--;

            // Indica qual nó foi removido o irmão ou o nó atual
            esq = 1;
        } else { // Merge com irmão da esquerda
            irmao = pai->filhos[atual->pai_posicao - 1];

            // Copia valor do pai para última posição do nó irmão
            irmao->key[irmao->n] = pai->key[atual->pai_posicao - 1];

            // Copia todos os elementos do nó atual para final do nó irmão
            int j = irmao->n + 1;
            for (int i = 0; i < atual->n; i++) {
                irmao->key[j] = atual->key[i];
                irmao->filhos[j] = atual->filhos[i];
                if (irmao->filhos[j] != NULL) {
                    irmao->filhos[j]->pai = irmao;
                    irmao->filhos[j]->pai_posicao = j;
                }
                j++;
            }

            // Copia último ponteiro do nó atual para o irmão
            irmao->filhos[j] = atual->filhos[atual->n];
            if (irmao->filhos[j] != NULL) {
                irmao->filhos[j]->pai = irmao;
                irmao->filhos[j]->pai_posicao = j;
            }

            // Irmão recebeu todos os nós de atual e um de seu pai
            irmao->n += atual->n + 1;

            // Libera estruturas do nó atual
            free(atual->key);
            free(atual->filhos);
            free(atual);

            arv->quantidadeNos--;
        }

        // Na operação de Merge o pai sempre perde um elemento
        pai->n--;

        // Verifica se desbalanceou o pai
        if (pai->n < (arv->ordem - 1) / 2 && arv->sentinela->filhos[0] != pai) {
            bal_remocao(arv, pai);
        } else if (pai->n == 0) {
            // Caso a raiz da árvore tenha ficado com 0 elementos é necessário fazer
            // atualização

            // Verifica qual nó deve se tornar a nova raiz
            if (esq) {
                arv->sentinela->filhos[0] = atual;
                atual->pai = arv->sentinela;
            } else {
                arv->sentinela->filhos[0] = irmao;
                irmao->pai = arv->sentinela;
            }

            // Libera estruturas da antiga raiz
            free(pai->key);
            free(pai->filhos);
            free(pai);

            arv->quantidadeNos--;
        }
    }
}

int procura_b_tree(b_tree *arv, int indice, no *raiz) {
    int i;

    // Condição de parada da função recursiva
    if (!arv || !raiz)
        return -1;

    for (i = 0; i < raiz->n; i++) {
        if (raiz->key[i].indice == indice)
            return raiz->key[i].valor;

        // Caso o índice buscado seja menor que o índice da chave do nó,
        // Chama a função recursivamente passando o filho como parâmetro
        if (indice < raiz->key[i].indice) {
            return procura_b_tree(arv, indice, raiz->filhos[i]);
        }
    }

    // Se o índice buscado for maior que todos os índices da raiz,
    // Chama a função recursivamente passando o último filho como parâmetro
    return procura_b_tree(arv, indice, raiz->filhos[raiz->n]);
}

void exclui_b_tree(struct b_tree *arv, struct no *raiz) {
    // Libera sentinela se existir
    if (arv->sentinela != NULL) {
        arv->sentinela->filhos[0] = NULL;
        free(arv->sentinela->filhos);
        free(arv->sentinela);
        arv->sentinela = NULL;
    }

    // Condição de parada da função recursiva
    if (raiz == NULL)
        return;

    // Se raiz não for folha, libera cada um de seus filhos antes
    if (!raiz->folha) {
        for (int i = 0; i <= raiz->n; i++) {
            exclui_b_tree(arv, raiz->filhos[i]);
        }
    }

    // Libera estruturas da raiz
    free(raiz->key);
    free(raiz->filhos);
    free(raiz);
}

no *raiz(b_tree *arv) {
    return arv->sentinela->filhos[0];
}

int nro_nos(b_tree *arv) {
    return arv->quantidadeNos;
}

void imprime_b_tree(no *raiz) {
    int i;
    // Condição de parada da função recursiva
    if (!raiz)
        return;

    // Imprime o índice do nó
    for (i = 0; i < raiz->n; i++) {
        printf("%d ", raiz->key[i].indice);
    }

    // Imprime a quantidade de chaves do nó e se o mesmo é ou não folha
    printf("n: %d ", raiz->n);
    if (raiz->folha) {
        printf("Folha");
    }
    printf("\n");

    // Chama a função recursivamente, para cada nó da árvore
    for (i = 0; i <= raiz->n; i++) {
        imprime_b_tree(raiz->filhos[i]);
    }
}

int processa_arquivo(b_tree *arv, char *nomeArquivo) {
    FILE *arq;
    arq = fopen(nomeArquivo, "r");

    if (!arq)
        return 0;

    int indice, linha = 1;
    char buffer[100];

    while (!feof(arq)) {
        // Lê índice a ser inserido na árvore
        fscanf(arq, "%d", &indice);

        // Insere uma chave na B Tree, contendo o índice e a linha do dado lido
        insere_b_tree(arv, indice, linha);

        // Pula para próxima linha no arquivo, descartando os dados após o índice
        fgets(buffer, 100, arq);

        // Incrementa a linha lida
        linha++;
    }

    return 1;
}
