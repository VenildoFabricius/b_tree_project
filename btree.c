#include <stdio.h>
#include <stdlib.h>
#include "btree.h"


b_tree *cria_b_tree(int ordem){
    b_tree *arv = (b_tree*) malloc(sizeof(b_tree));
    arv->ordem = ordem;
    arv->sentinela = (no*) malloc(sizeof(no));
    arv->sentinela->ponteiros = (no**) malloc(sizeof(no*));
    arv->sentinela->ponteiros[0] = NULL;

    // n = -1 indica que é a sentinela
    arv->sentinela->n = -1;
    return arv;
}

int insere_b_tree(b_tree *arv, int indice, int linha){
    no *raiz, *atual;
    int achou, i;

    if(!arv)
        return 0;

    if(!arv->sentinela->ponteiros[0]){
        raiz = (no*) malloc(sizeof(no));
        arv->sentinela->ponteiros[0] = raiz;

        raiz->vet = (int**) malloc(arv->ordem*sizeof(int*));
        for(i = 0; i < arv->ordem; i++){
            raiz->vet[i] = (int*) malloc(2*sizeof(int));
        }
        raiz->vet[0][0] = indice;
        raiz->vet[0][1] = linha;
        raiz->pai = arv->sentinela;
        raiz->folha = 1;
        raiz->n = 1;

        //Aloca vetor de ponteiros do nó de tamanho [m+1]
        raiz->ponteiros = (no**) malloc((arv->ordem+1)*sizeof(no*));

        for(i = 0; i <=arv->ordem; i++){
            raiz->ponteiros[i] = NULL;
        }
        return 1;
    }

    atual = arv->sentinela->ponteiros[0];


    //Procura nó folha para inserir elemento
    while(atual->folha != 1){

        achou = 0;

        //Percorre vetor até que índice seja maior que posição do vetor
        for(i = 0; i < atual->n; i++){
            if(indice < atual->vet[i][0]){
                atual = atual->ponteiros[i];
                achou = 1;
                break;
            }
        }

        //Caso indice seja maior que todos elementos do vetor nó atual recebe último ponteiro
        if(!achou){
            atual = atual->ponteiros[atual->n];
            achou = 0;
        }
    }

    //Insere ordenado no nó
    for(i = atual->n; i>0; i--){
        if(indice > atual->vet[i-1][0]){
            break;
        }
        atual->vet[i][0] = atual->vet[i-1][0];
        atual->vet[i][1] = atual->vet[i-1][1];
    }
    atual->vet[i][0] = indice;
    atual->vet[i][1] = linha;
    atual->n++;

    // Verifica se nó atingiu limite de tamanho
//    if(atual->n == arv->ordem){
//        bal_insercao(arv, atual);
//    }

    return 1;
}