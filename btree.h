#ifndef BTREE_H
#define BTREE_H

typedef struct chave{           // Estrutura para armazenar a chave
    int indice;                 // Guarda o índice da chave
    int valor;                  // Guarda o valor da chave
} chave;

typedef struct no {
    chave *key;                 // Vetor que armazena as chaves do nó
    struct no **filhos;         // Vetor de filhos para os filhos
    struct no *pai;             // Ponteiro para o pai do nó
    int folha;                  // Inteiro que informa se o nó é folha
    int n;                      // Inteiro que informa a quantidade atual de chaves no nó
    int pai_posicao;            // Inteiro que guarda a posição do nó em relação ao pai
} no;

typedef struct b_tree {
    no *sentinela;              // Ponteiro para a raiz da árvore
    int ordem;                  // Inteiro que informa a ordem da árvore
    int quantidadeNos;          // Inteiro para armazenar a quantidade de nós da árvore
} b_tree;


// Função que cria a B Tree com a ordem passada como parâmetro
b_tree *cria_b_tree(int ordem);

// Função que insere um elemento na B Tree
int insere_b_tree(b_tree *arv, int indice, int valor);

// Função para balancear árvore na inserção
void bal_insercao(b_tree *arv, no *atual);

// Função para remover um elemento da B Tree
int remove_b_tree(b_tree *arv, int indice);

// Função para balancear árvore na remoção
void bal_remocao(b_tree *arv, no *atual);

// Função para procurar elementos na B Tree
int procura_b_tree(b_tree *arv, int indice, no *raiz);

// Função para excluir a B Tree
void exclui_b_tree(b_tree *arv, no *raiz);

// Função para retornar a raiz da B Tree
no *raiz(b_tree *arv);

// Função para retornar a quantidade de nós da B Tree
int nro_nos(b_tree *arv);

// Função para imprimir a B Tree
void imprime_b_tree(no *raiz);

// Função para carregar o arquivo com os dados
int processa_arquivo(b_tree *arv, char *nomeArquivo);

#endif // BTREE_H
