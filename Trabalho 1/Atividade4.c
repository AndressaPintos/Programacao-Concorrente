// objetivo do programa: calcular o produto interno entre dois vetores de n dimensões

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 
#include "timer.h"

//vetor de elementos - variaveis globais
float *vetorA;
float *vetorB;

//estrutura de dados para passar argumentos para a thread
typedef struct {
   //tamanho do vetor
   long int n;
   //numero de threads 
   short int nthreads;
   //identificador da thread
   short int id;
} t_args;  


//funcao executada pelas threads
void *prodInterno (void *args) {
  t_args *arg = (t_args*) args; //argumentos da thread
  int ini, fim, fatia; //auxiliares para divisao do vetor em blocos
  float prodI_local=0, *ret;

  fatia = arg->n / arg->nthreads; 
  ini = arg->id * fatia; 
  fim = ini + fatia;

  if (arg->id == (arg->nthreads-1)) 
    fim = arg->n; 

  //realiza o produto interno de parte dos vetores
  for(int i=ini; i<fim; i++) {
     prodI_local += vetorA[i]*vetorB[i];
  }

  //retorna o resultado do produto interno
  ret = (float*) malloc(sizeof(float));
  if (ret!=NULL)
    *ret = prodI_local;
  else 
    printf("Erro na alocação de espaço para o retorno\n");
  pthread_exit((void*) ret);
}

int main(int argc, char *argv[]) {
  long int n; 
  short int nthreads; 
  FILE *arq; 
  size_t retArq,retArq2;
  float erro,prodI=0;//produto interno encontrado de forma concorrente
  double start, end, delta,prodI_original; //produto interno registrado no arquivo
  float *retorno; //auxiliar para retorno das threads

  pthread_t *tid_sistema;

  if(argc < 3){ 
    printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]); 
    exit(-1); 
    }

  //abre o arquivo de entrada com os valores para serem somados
  arq = fopen(argv[1], "rb");
  if(arq==NULL){ 
    printf("Erro na abertura do arquivo\n"); exit(-1); }

  //le o tamanho do vetor (primeira linha do arquivo)
  retArq = fread(&n, sizeof(long int), 1, arq);
  if(!retArq) {
     fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
     return 3;
  }

  //aloca espaco de memoria e carrega o vetor de entrada
  vetorA = (float*) malloc (sizeof(float) * n);
  vetorB = (float*) malloc (sizeof(float) * n);
  if((vetorA==NULL) || (vetorB==NULL)){
     printf("--ERRO: malloc()\n"); 
     exit(-1); 
    }
  retArq = fread(vetorA, sizeof(float), n, arq);
  retArq2 = fread(vetorB, sizeof(float), n, arq);
  if((retArq < n) || (retArq2 < n)) {
     fprintf(stderr, "Erro de leitura dos elementos do vetor\n");
     return 4;
  }

  //le o produto interno encontrado no programa sequencial
  retArq = fread(&prodI_original, sizeof(long int), 1, arq);
  if(!retArq) {
     fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
     return 5;
  }

  //le o numero de threads da entrada do usuario 
  nthreads = atoi(argv[2]);
  //limita o numero de threads ao tamanho do vetor
  if(nthreads>n) 
    nthreads = n;

  //aloca espaco para o vetor de identificadores das threads no sistema
  tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if(tid_sistema==NULL){
    printf("Erro, não foi possível alocar memória para o vetor de threads\n"); 
    exit(-1); 
    }

    GET_TIME(start);//começa a contagem de tempo
  //cria as threads
  for(long int i=0; i<nthreads; i++) {
    t_args *args;
    args = (t_args*) malloc(sizeof(t_args));
    if(args==NULL) {    
       printf("Erro na alocação de memória das threads\n");
    exit(-1);
    }
    args->n = n;
    args->nthreads = nthreads;
    args->id = i;
    if (pthread_create(&tid_sistema[i], NULL, prodInterno, (void*) args)) {
       printf("Erro na criação das threads\n"); exit(-1);
    }
  }

  //espera todas as threads terminarem e calcula a soma total das threads
  for(int i=0; i<nthreads; i++) {
     if (pthread_join(tid_sistema[i], (void *) &retorno)) {
        printf("Erro no retorno das threads\n"); 
        exit(-1);
     }
     prodI += *retorno;
     free(retorno);
  }
  GET_TIME(end); //termina a contagem de tempo
  delta = end-start; //calcula o intervalo de tempo para processamento do produto interno

  //imprime os resultados do cálculo concorrente em comparação com o cálculo sequencial:
  erro = abs((prodI_original-prodI)/prodI_original);
  printf("\n");
  printf("produto Interno original: %f\nProduto interno concorrente: %f\n",prodI_original,prodI);
  printf("Erro:   %e\n", erro); 
  printf("Tempo de execução: %.6f seg\n",delta);

  //desaloca os espacos de memoria
  free(vetorA);
  free(vetorB);
  free(tid_sistema);
  //fecha o arquivo
  fclose(arq);

  return 0;
}

