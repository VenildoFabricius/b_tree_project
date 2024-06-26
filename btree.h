#ifndef BTREE_H
#define BTREE_H

typedef struct no {
    int **vet;                  // Vetor que armazena as chaves do nó
    struct no **ponteiros;      // Vetor de ponteiros para os filhos
    struct no *pai;             // Ponteiro para o pai do nó
    int folha;                  // Inteiro que informa se o nó é folha
    int n;                      // Inteiro que informa a quantidade atual de chaves no nó
} no;


typedef struct bTree {
    no *sentinela;              // Ponteiro para a raiz da árvore
    int ordem;                  // Inteiro que informa a ordem da árvore
} b_tree;


// Função que cria a B Tree com a ordem passada como parâmetro
b_tree *cria_b_tree(int ordem);

// Função que insere um elemento na B Tree
int insere_b_tree(b_tree *arv, int indice, int linha);

// Função para balancear árvore na inserção
void bal_insercao(b_tree *arv, no *atual);

// Função para remover um elemento da B Tree
int remove_b_tree(b_tree *arv, int indice);

// Função para balancear árvore na remoção
void bal_remocao(b_tree *arv, no *atual);

// Função para procurar elementos na B Tree
int procura_b_tree(b_tree *arv, int indice);


#endif // BTREE_H
