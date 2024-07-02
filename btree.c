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

        // Aloca as chaves do nó, com ordem chaves. A posição extra permite alocar uma chave além da permitida e
        // realizar a operação de split posteriormente
        raiz->key = (chave *)malloc(arv->ordem * sizeof(chave));
        // Aloca o vetor de filhos, com tamanho ordem + 1. Mesmo caso da quantidade de chaves.
        raiz->filhos = (no **)malloc((arv->ordem + 1) * sizeof(no *));
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
            // Os índices são copiados uma posição para a frente do vetor a cada iteração
            pai->key[i] = pai->key[i - 1];
            // A partir da posição atual do nó, os vetores dos filhos do pai são copiados uma posição para a frente
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
