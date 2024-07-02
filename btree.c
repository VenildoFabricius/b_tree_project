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
    // Aloca o vetor do filho da sentinela, com tamanho único
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

        // Aloca as chaves do nó, com ordem-1 chaves
        raiz->key = (chave *)malloc((arv->ordem - 1) * sizeof(chave));
        // Aloca o vetor de filhos, com tamanho ordem
        raiz->filhos = (no **)malloc((arv->ordem) * sizeof(no *));
        // Para cada posição do vetor de filhos, atribui NULL, pois a raiz ainda não nenhum filho
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
                // Encontrou o vetor de filhos onde a inserção deve ser feita
                atual = atual->filhos[i];
                achou = 1;
                // Sai do loop
                break;
            }
        }
        // Se o índice da chave a ser inserida for maior que o maior índice do nó atual, a chave pertence a um filho
        // direito do nó atual e o loop anterior não pôde encontrar
        if (!achou) {
            // A chave deve ser inserida no vetor com chaves maiores que o maior índice do nó atual
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
        // Se não, os índices são copiados uma posição para a frente do vetor até que a posição de inserção seja encontrada
        atual->key[i] = atual->key[i - 1];
    }
    // Encontrada a posição de inserção, atribui-se o índice e o valor passados como parâmetros à respectiva chave
    atual->key[i].indice = indice;
    atual->key[i].valor = valor;
    // Incrementa a quantidade de chaves do nó
    atual->n++;

    // Se a quantidade de chaves do nó atingiu o limite máximo (ordem - 1), é necessário fazer o balanceamento
    if (atual->n == arv->ordem - 1) {
        bal_insercao(arv, atual);
    }

    return 1;
}

void bal_insercao(b_tree *arv, no *atual) {
    int i, j = 0;
    no *pai;

    // Operações de SPLIT

    // Verifica se atual é a raiz
    if (arv->sentinela->filhos[0] == atual) {

        // Cria nova raiz
        pai = (no *)malloc(sizeof(no));
        pai->key = (chave *)malloc(arv->ordem * sizeof(chave));
        pai->filhos = (no **)malloc((arv->ordem + 1) * sizeof(no *));
        for (i = 0; i <= arv->ordem; i++) {
            pai->filhos[i] = NULL;
        }

        // Cria irmão
        no *irmao = (no *)malloc(sizeof(no));
        irmao->key = (chave *)malloc(arv->ordem * sizeof(chave));
        irmao->filhos = (no **)malloc((arv->ordem + 1) * sizeof(no *));
        for (i = 0; i <= arv->ordem; i++) {
            irmao->filhos[i] = NULL;
        }

        // Atualiza valores do pai
        pai->folha = 0;
        pai->n = 1;
        pai->pai = arv->sentinela;
        arv->sentinela->filhos[0] = pai;
        pai->filhos[0] = atual;
        pai->filhos[1] = irmao;

        // Move valor central do nó atual para nova raiz
        pai->key[0] = atual->key[(arv->ordem - 1) / 2];

        // Atualiza valores do irmão
        irmao->folha = atual->folha;
        irmao->n = arv->ordem / 2;
        irmao->pai = pai;
        irmao->posPai = 1;

        // Insere elementos do nó atual no nó irmão criado
        for (i = ((arv->ordem - 1) / 2) + 1; i < arv->ordem; i++) {
            irmao->key[j] = atual->key[i];
            irmao->filhos[j] = atual->filhos[i];
            if (atual->filhos[i] != NULL) {
                atual->filhos[i]->pai = irmao;
                atual->filhos[i]->posPai = j;
                atual->filhos[i] = NULL;
            }
            j++;
        }
        // Copia último ponteiro de atual para irmão
        irmao->filhos[j] = atual->filhos[i];
        if (atual->filhos[i]) {
            atual->filhos[i]->pai = irmao;
            atual->filhos[i]->posPai = j;
            atual->filhos[i] = NULL;
        }

        // Atualizando atual
        atual->n = (arv->ordem - 1) / 2;
        atual->pai = pai;
        atual->posPai = 0;

        arv->quantidadeNos += 2;
    } else {
        // Copia valores e filhos uma posição para trás no nó pai a partir da
        // posição que o nó atual estava
        pai = atual->pai;
        for (i = pai->n; i > atual->posPai; i--) {
            pai->key[i] = pai->key[i - 1];
            pai->filhos[i + 1] = pai->filhos[i];
            pai->filhos[i + 1]->posPai++;
        }

        // Move valor central do nó atual para nó pai
        pai->key[atual->posPai] = atual->key[(arv->ordem - 1) / 2];

        // Cria irmão
        no *irmao = (no *)malloc(sizeof(no));
        irmao->key = (chave *)malloc(arv->ordem * sizeof(chave));
        irmao->filhos = (no **)malloc((arv->ordem + 1) * sizeof(no *));
        for (i = 0; i <= arv->ordem; i++) {
            irmao->filhos[i] = NULL;
        }

        // Faz pai apontar para o irmão criado uma posição após o nó atual no vetor
        // de filhos do pai
        pai->filhos[atual->posPai + 1] = irmao;

        // Copia valores de atual para irmão
        irmao->folha = atual->folha;
        irmao->n = arv->ordem / 2;
        irmao->pai = pai;
        irmao->posPai = atual->posPai + 1;

        for (i = ((arv->ordem - 1) / 2) + 1; i < arv->ordem; i++) {
            irmao->key[j] = atual->key[i];
            irmao->filhos[j] = atual->filhos[i];
            if (irmao->filhos[j] != NULL) {
                irmao->filhos[j]->pai = irmao;
                irmao->filhos[j]->posPai = j;
                atual->filhos[i] = NULL;
            }
            j++;
        }
        // Copia último ponteiro de atual para irmão
        irmao->filhos[j] = atual->filhos[i];
        if (atual->filhos[i]) {
            atual->filhos[i]->pai = irmao;
            atual->filhos[i]->posPai = j;
            atual->filhos[i] = NULL;
        }

        atual->n = (arv->ordem - 1) / 2;

        pai->n++;

        // Verifica se desbalanceou o pai
        if (pai->n == arv->ordem) {
            bal_insercao(arv, pai);

            // Atualiza nó pai caso ele tenha mudado pela chamada da função
            pai = atual->pai;
        }

        arv->quantidadeNos++;
    }
}
