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
    // Inicializa a quantidade de nós da árvore
    arv->quantidadeNos = 0;
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
    // Se a árvore não foi alocada ou se está vazia, não é possível fazer a remoção
    if (!arv || !arv->sentinela->filhos[0])
        return 0;

    int pos = -1, i;
    no *atual, *sucesr;

    // Para remover, é necessário encontrar o índice. A busca começa da raiz
    atual = arv->sentinela->filhos[0];

    // Percorre as chaves do nó atual até encontrar o índice a ser removido
    do {
        for (i = 0; i < atual->n; i++) {
            if (indice == atual->key[i].indice) {
                // Se encontrou, a variável pos recebe a posição do índice no nó atual, e a busca é encerrada
                pos = i;
                break;
            }

            // Se não, a busca continua em um dos filhos do nó atual
            if (indice < atual->key[i].indice) {
                atual = atual->filhos[i];
                // Variável i é inicializada para realizar a busca dentro do nó atual
                i = -1;
                break;
            }
        }

        // Caso atual ainda seja um nó válido e o índice buscado não tenha sido encontrado, a busca é realizada no nó
        // com os maiores índices
        if (atual && i == atual->n)
            atual = atual->filhos[i];
        // A busca continua até encontrar o índice ou até atingir um nó nulo
    } while (pos == -1 && atual);

    // Se a busca atingiu um nó nulo, a função retorna 0, indicando que o índice não está na árvore
    if (!atual)
        return 0;

    // Caso a chave a ser removida esteja em um nó folha
    if (atual->folha) {
        // As chaves são movidas uma posição para trás dentro do nó, a partir do elemento a ser removido
        for (i = pos; i < atual->n - 1; i++) {
            atual->key[i] = atual->key[i + 1];
        }
        // A quantidade de chaves do nó é decrementada
        atual->n--;

        // Se a quantidade de chaves remanescentes no nó for menor que a mínima e o nó não é raiz, o nó foi desbalanceado
        if (atual->n < (arv->ordem - 1) / 2 && arv->sentinela->filhos[0] != atual)
            // Chama a função de balanceamento
            bal_remocao(arv, atual);
        // Se a quantidade de chaves do nó atual for igual a zero, significa que a última chave da árvore foi removida
        else if (atual->n == 0) {
            // Libera a memória alocada para a árvore
            free(atual->key);
            free(atual->filhos);
            free(atual);

            // Inicializa a raiz
            arv->sentinela->filhos[0] = NULL;
            // Decrementa a quantidade de nós
            arv->quantidadeNos--;
        }

    } // Caso a chave a ser removida esteja em um nó interno
    else {
        // Procura pela chave sucessora da chave a ser removida
        sucesr = atual->filhos[pos + 1];
        // A busca pelo sucessor continua até encontrar um nó folha
        while (sucesr->filhos[0])
            sucesr = sucesr->filhos[0];

        // Encontrada a chave sucessora, ela é copiada para a chave removida
        atual->key[pos] = sucesr->key[0];

        // As chaves do nó da sucessora são movidas uma posição para trás
        for (i = 0; i < sucesr->n - 1; i++) {
            sucesr->key[i] = sucesr->key[i + 1];
        }
        // A quantidade de chaves do nó que cedeu a sucessora é decrementada
        sucesr->n--;

        // Se a quantidade de chaves remanescentes no nó for menor que a mínima, o nó foi desbalanceado
        if (sucesr->n < (arv->ordem - 1) / 2) {
            // Chama a função de balanceamento
            bal_remocao(arv, sucesr);
        }
    }

    return 1;
}

void bal_remocao(b_tree *arv, no *atual) {
    int esq = 0, dir = 0, i, j;
    no *irmao, *pai;

    pai = atual->pai;

    // Se o nó desbalanceado não for o menor filho do pai e o seu irmão esquerdo tiver uma quantidade de chaves
    // maior que a mínima
    if (atual->pai_posicao > 0 && pai->filhos[atual->pai_posicao - 1] &&
        pai->filhos[atual->pai_posicao - 1]->n > (arv->ordem - 1) / 2) {
        // É possível realizar a rotação esquerda
        esq = 1;
        // Variável irmão recebe o irmão esquerdo do nó desbalanceado
        irmao = pai->filhos[atual->pai_posicao - 1];
    // Se não, se o nó desbalanceado possuir irmão direito e ele tiver uma quantidade de chaves maior que a mínima
    } else if (pai->n >= atual->pai_posicao + 1 && pai->filhos[atual->pai_posicao + 1]->n > (arv->ordem - 1) / 2) {
        // É possível realizar a rotação direita
        dir = 1;
        // Variável irmão recebe o irmão direito do nó desbalanceado
        irmao = pai->filhos[atual->pai_posicao + 1];
    }

    // Se é possível realizar a rotação esquerda
    if (esq) {
        // Move as chaves e os filhos do nó atual uma posição para a frente, liberando a primeira posição do nó
        for (i = atual->n; i > 0; i--) {
            atual->key[i] = atual->key[i - 1];
            atual->filhos[i + 1] = atual->filhos[i];
            // Se os filhos não forem nulos, incrementa a posição deles em relação ao pai
            if (atual->filhos[i + 1])
                atual->filhos[i + 1]->pai_posicao++;
        }
        // O mesmo é feito para a posição 0 do nó
        atual->filhos[1] = atual->filhos[0];
        if (atual->filhos[1])
            atual->filhos[1]->pai_posicao++;

        // A primeira posição do nó atual, agora vazia, recebe do pai, a chave anterior à chave removida
        atual->key[0] = pai->key[atual->pai_posicao - 1];

        // A última chave do irmão esquerdo é copiada para a posição do pai que cedeu a chave para o nó atual
        pai->key[atual->pai_posicao - 1] = irmao->key[irmao->n - 1];

        // Os filhos da última chave do irmão, que foi transferida para o pai, passam a ser os primeiros filhos do nó atual
        atual->filhos[0] = irmao->filhos[irmao->n];
        // Atualiza o pai dos filhos que foram transferidos para o nó atual, bem como a posição deles em relação ao pai
        if (atual->filhos[0]) {
            atual->filhos[0]->pai = atual;
            atual->filhos[0]->pai_posicao = 0;
        }
        // Incrementa a quantidade de chaves do nó atual
        atual->n++;
        // e decrementa a quantidade de chaves do irmão que transferiu a chave
        irmao->n--;

    } // Se é possível realizar a rotação direita
    else if (dir) {
        // A posição "extra" do nó atual recebe do pai, a chave sucessora da chave removida
        atual->key[atual->n] = pai->key[atual->pai_posicao];

        // A primeira chave do irmão da direita é copiada para o pai, no lugar da chave transferida para o nó atual
        pai->key[atual->pai_posicao] = irmao->key[0];

        // Os filhos da primeira chave do irmão, que foi transferida para o pai, passam a ser os últimos filhos do nó atual
        atual->filhos[atual->n + 1] = irmao->filhos[0];
        // Atualiza o pai dos filhos que foram transferidos para o nó atual, bem como a posição deles em relação ao pai
        if (atual->filhos[atual->n + 1]) {
            atual->filhos[atual->n + 1]->pai = atual;
            atual->filhos[atual->n + 1]->pai_posicao = atual->n + 1;
        }

        // Como a primeira posição do irmão ficou vazia, move todas as chaves e filhos deste nó uma posição para trás
        for (i = 0; i < irmao->n - 1; i++) {
            irmao->key[i] = irmao->key[i + 1];
            irmao->filhos[i] = irmao->filhos[i + 1];
            // Atualiza a posição dos filhos em relação ao pai, caso estes não sejam nulos
            if (irmao->filhos[i])
                irmao->filhos[i]->pai_posicao--;
        }

        // O mesmo é feito para a posição 0 do nó
        irmao->filhos[irmao->n - 1] = irmao->filhos[irmao->n];
        if (irmao->filhos[irmao->n - 1])
            irmao->filhos[irmao->n - 1]->pai_posicao--;

        // Incrementa a quantidade de chaves do nó atual
        atual->n++;
        // e decrementa a quantidade de chaves do irmão que transferiu a chave
        irmao->n--;
    }
    // Se não for possível realizar rotações, é necessário realizar a operação de MERGE
    else {
        // Se o nó atual não for o maior filho do pai, faz o merge com o irmão da direita
        if (atual->pai_posicao < pai->n) {
            // Variável irmão recebe o irmão da direita
            irmao = pai->filhos[atual->pai_posicao + 1];

            // A última posição do nó atual recebe do pai, a chave sucessora da chave removida
            atual->key[atual->n] = pai->key[atual->pai_posicao];

            // Como uma chave do pai foi movida para um de seus filhos, move-se uma posição para trás,
            // todas as chaves e filhos posteriores à chave que foi movida
            for (i = atual->pai_posicao + 1; i < pai->n; i++) {
                pai->key[i - 1] = pai->key[i];
                pai->filhos[i] = pai->filhos[i + 1];
                if (pai->filhos[i])
                    pai->filhos[i]->pai_posicao--;
            }

            // Copia as chaves e os filhos do irmão para o nó atual
            // Como a quantidade de chaves do nó atual ainda não foi incrementada, e este nó já recebeu a chave do pai,
            // o ponto de inserção de novos elementos (j) será a quantidade de chaves no nó +1
            j = atual->n + 1;
            for (i = 0; i < irmao->n; i++) {
                atual->key[j] = irmao->key[i];
                atual->filhos[j] = irmao->filhos[i];
                // Atualiza o pai dos filhos, caso não sejam nulos, bem como a posição deles em relação ao pai
                if (atual->filhos[j]) {
                    atual->filhos[j]->pai = atual;
                    atual->filhos[j]->pai_posicao = j;
                }
                j++;
            }

            // O mesmo é feito para o último elemento
            atual->filhos[j] = irmao->filhos[irmao->n];
            if (atual->filhos[j]) {
                atual->filhos[j]->pai = atual;
                atual->filhos[j]->pai_posicao = j;
            }

            // Incrementa a quantidade de chaves do nó atual com a quantidade de chaves recebidas do irmão
            // mais a chave recebida do pai
            atual->n += irmao->n + 1;

            // Libera a memória alocada para o irmão, que agora formou apenas um nó com o nó atual
            free(irmao->key);
            free(irmao->filhos);
            free(irmao);

            // Decrementa a quantidade de nós da árvore
            arv->quantidadeNos--;

            // Reutilização da variável esq para informar que o nó removido foi o irmão. Ao fim da operação de merge,
            // caso a raiz tenha ficado sem nenhuma chave, ela deve ser atualizada com o nó atual ou irmão
            esq = 1;
        }
        // Se não for possível o merge com o irmão da direita, faz com o irmão da esquerda
        else {
            // variável irmão recebe o irmão da esquerda
            irmao = pai->filhos[atual->pai_posicao - 1];

            // A última posição do nó irmão recebe do pai, a chave anterior à chave removida
            irmao->key[irmao->n] = pai->key[atual->pai_posicao - 1];

            // Copia as chaves e os filhos do nó atual para o final do irmão esquerdo
            j = irmao->n + 1;
            for (i = 0; i < atual->n; i++) {
                irmao->key[j] = atual->key[i];
                irmao->filhos[j] = atual->filhos[i];
                // Atualiza o pai dos filhos, caso não sejam nulos, bem como a posição deles em relação ao pai
                if (irmao->filhos[j]) {
                    irmao->filhos[j]->pai = irmao;
                    irmao->filhos[j]->pai_posicao = j;
                }
                j++;
            }

            // O mesmo é feito para o último elemento
            irmao->filhos[j] = atual->filhos[atual->n];
            if (irmao->filhos[j]) {
                irmao->filhos[j]->pai = irmao;
                irmao->filhos[j]->pai_posicao = j;
            }

            // Incrementa a quantidade de chaves do irmão com a quantidade de chaves recebidas do nó atual
            // mais a chave recebida do pai
            irmao->n += atual->n + 1;

            // Libera a memória alocada para o nó atual, que agora formou apenas um nó com o irmão
            free(atual->key);
            free(atual->filhos);
            free(atual);

            // Decrementa a quantidade de nós da árvore
            arv->quantidadeNos--;
        }

        // Na operação de Merge o pai sempre perde um elemento
        // Decrementa a quantidade de chaves do pai, que na operação de merge perde um elemento
        pai->n--;

        // Se a quantidade de chaves remanescentes no pai for menor que a mínima e o pai não for raiz, indica que
        // o pai foi desbalanceado
        if (pai->n < (arv->ordem - 1) / 2 && arv->sentinela->filhos[0] != pai)
            // Chama a função recursivamente para balanceamento do pai
            bal_remocao(arv, pai);

        // Se o pai for a raiz e não sobrou nenhuma chave neste nó, a raiz deve ser atualizada.
        else if (pai->n == 0) {
            // Se a variável esq foi alterada, indica que o irmão foi removido no merge, portanto, a nova raiz
            // passa a ser o nó atual
            if (esq) {
                arv->sentinela->filhos[0] = atual;
                atual->pai = arv->sentinela;
            }
            // Se não, a nova raiz passa a ser o irmão
            else {
                arv->sentinela->filhos[0] = irmao;
                irmao->pai = arv->sentinela;
            }

            // Libera a memória alocada para o nó pai, que era a raiz
            free(pai->key);
            free(pai->filhos);
            free(pai);

            // Decrementa a quantidade de nós da árvore
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
    while (fscanf(arq, "%d", &indice) == 1) {
        // Insere na B Tree, o índice e a linha em que os dados estão no arquivo
        insere_b_tree(arv, indice, linha);

        // Pula para a próxima linha no arquivo
        // Consome o restante da linha
        fscanf(arq, "%*[^\n]");
        fgetc(arq);

        // Aumenta o número da linha no arquivo
        linha++;
    }

    // Fecha o arquivo
    fclose(arq);
    return 1;
}
