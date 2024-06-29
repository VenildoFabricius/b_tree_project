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

void bal_insercao(b_tree *arv, no *atual){
    int i, j = 0;
    no *pai;

    //Operações de SPLIT

    //Verifica se atual é a raiz
    if (arv->sentinela->ponteiros[0] == atual) {

        //Cria nova raiz
        pai = (no*) malloc(sizeof(no));
        pai->vet = (int**) malloc(arv->ordem*sizeof(int*));
        for (i = 0; i < arv->ordem; i++){
            pai->vet[i] = (int *) malloc(2 * sizeof(int));
        }
        pai->ponteiros = (no**) malloc((arv->ordem+1)*sizeof(no*));
        for (i = 0; i <= arv->ordem; i++){
            pai->ponteiros[i] = NULL;
        }

        //Cria irmão
        no *irmao = (no*) malloc(sizeof(no));
        irmao->vet = (int**) malloc(arv->ordem * sizeof(int*));
        for (i = 0; i < arv->ordem; i++){
            irmao->vet[i] = (int*) malloc(2*sizeof(int));
        }
        irmao->ponteiros = (no**) malloc((arv->ordem+1) * sizeof(no*));
        for (i = 0; i <= arv->ordem; i++){
            irmao->ponteiros[i] = NULL;
        }

        //Atualiza valores do pai
        pai->folha = 0;
        pai->n = 1;
        pai->pai = arv->sentinela;
        arv->sentinela->ponteiros[0] = pai;
        pai->ponteiros[0] = atual;
        pai->ponteiros[1] = irmao;

        //Move valor central do nó atual para nova raiz
        pai->vet[0][0] = atual->vet[(arv->ordem-1)/2][0];
        pai->vet[0][1] = atual->vet[(arv->ordem-1)/2][1];

        //Atualiza valores do Irmão
        irmao->folha = atual->folha;
        irmao->n = ((arv->ordem)/2);
        irmao->pai = pai;
        irmao->posPai = 1;

        //Insere elementos do nó atual no nó irmão criado
        for (i = ((arv->ordem-1)/2)+1; i < arv->ordem; i++){
            irmao->vet[j][0] = atual->vet[i][0];
            irmao->vet[j][1] = atual->vet[i][1];
            irmao->ponteiros[j] = atual->ponteiros[i];
            if (atual->ponteiros[i]){
                atual->ponteiros[i]->pai = irmao;
                atual->ponteiros[i]->posPai = j;
                atual->ponteiros[i] = NULL;
            }
            j++;
        }

        //Copia última ponteiro de atual para irmão
        irmao->ponteiros[j] = atual->ponteiros[i];
        if (atual->ponteiros[i]){
            atual->ponteiros[i]->pai = irmao;
            atual->ponteiros[i]->posPai = j;
            atual->ponteiros[i] = NULL;
        }

        //Atualizando atual
        atual->n = (arv->ordem-1)/2;
        atual->pai = pai;
        atual->posPai = 0;

        arv->qtd_nos += 2;
    }

    else {
        /*Copia valores e ponteiros uma posição para trás no nó pai a partir da posição que o
        nó atual estava*/
        pai = atual->pai;
        for (i = pai->n; i > atual->posPai; i--) {
            pai->vet[i][0] = pai->vet[i-1][0];
            pai->vet[i][1] = pai->vet[i-1][1];
            pai->ponteiros[i+1] = pai->ponteiros[i];
            pai->ponteiros[i+1]->posPai++;
        }

        //Move valor central do nó atual para nó pai
        pai->vet[atual->posPai][0] = atual->vet[(arv->ordem-1)/2][0];
        pai->vet[atual->posPai][1] = atual->vet[(arv->ordem-1)/2][1];

        //Cria irmão
        no *irmao = (no*) malloc(sizeof(no));
        irmao->vet = (int**) malloc(arv->ordem*sizeof(int*));
        for (i = 0; i < arv->ordem; i++){
            irmao->vet[i] = (int*) malloc(2 * sizeof(int));
        }
        irmao->ponteiros = (no**) malloc((arv->ordem+1)*sizeof(no*));
        for (i = 0; i <= arv->ordem; i++){
            irmao->ponteiros[i] = NULL;
        }

        /*Faz pai apontar para o irmão criado uma posição após o nó atual no vetor de ponteiros
        do pai*/
        pai->ponteiros[atual->posPai+1] = irmao;

        //Copia valores de atual para irmão
        irmao->folha = atual->folha;
        irmao->n = (arv->ordem)/2;
        irmao->pai = pai;
        irmao->posPai = atual->posPai+1;

        for (i = ((arv->ordem-1)/2)+1; i < arv->ordem; i++){
            irmao->vet[j][0] = atual->vet[i][0];
            irmao->vet[j][1] = atual->vet[i][1];
            irmao->ponteiros[j] = atual->ponteiros[i];
            if (irmao->ponteiros[j]){
                irmao->ponteiros[j]->pai = irmao;
                irmao->ponteiros[j]->posPai = j;
                atual->ponteiros[i] = NULL;
            }
            j++;
        }

        //Copia última ponteiro de atual para irmão
        irmao->ponteiros[j] = atual->ponteiros[i];
        if (atual->ponteiros[i]){
            atual->ponteiros[i]->pai = irmao;
            atual->ponteiros[i]->posPai = j;
            atual->ponteiros[i] = NULL;
        }

        atual->n = (arv->ordem-1)/2;

        pai->n++;

        //Verifica se desbalanceou o pai
        if(pai->n == arv->ordem){
            bal_insercao(arv, pai);

            //Atualiza nó pai caso ele tenha mudado pela chamada da função
            pai = atual->pai;
        }

        arv->qtd_nos++;
    }
}
