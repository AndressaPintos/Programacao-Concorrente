//Objetivo do programa: gerar 2 vetores de floats de n dimensões

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOG 

//Calcula o produto interno de 2 vetores
float prodInterno(float *A, float *B, long int dim){
    int soma=0;
    for(int i=0; i<dim; i++){
        soma += A[i]*B[i];
    }
    return soma;
}

//aloca memoria para o vetor
void alocaMemoria(float **vetor, long int dim){
  *vetor = (float*) malloc(sizeof(float) * dim);
  if(!vetor) {
     fprintf(stderr, "Erro de alocao da memoria do vetor\n");
     return;
  }
}

//preenche o vetor com valores float aleatorios
void preencheVetor(float *vetor,long int dim){
    for(int i=0; i<dim; i++) {
        vetor[i] = (float)(rand() % 10) * 1.8 - 5;
   }
}

int main(int argc, char*argv[]) {
   float *vetorA, *vetorB; 
   long int n; 
   double prodI=0;
   FILE * descritorArquivo; //descritor do arquivo de saida
   size_t ret1,ret2; //retorno da funcao de escrita no arquivo de saida
   srand(time(NULL));

   //recebe os argumentos de entrada
   if(argc < 3) {
      fprintf(stderr, "Digite: %s <dimensao do vetor> <nome do arquivo de saida>\n", argv[0]);
      return 1;
   }
   n = atoi(argv[1]);
   alocaMemoria(&vetorA,n);
   alocaMemoria(&vetorB,n);
   preencheVetor(vetorA,n);
   preencheVetor(vetorB,n);
   prodI = prodInterno(vetorA,vetorB,n);
   

   //imprimir na saida padrao o vetor gerado
   #ifdef LOG
   fprintf(stdout, "%ld\n", n);
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "[%f] ",vetorA[i]);
   }
   puts("\n");
   for(long int i=0; i<n; i++) {
    fprintf(stdout, "[%f] ",vetorB[i]);
    }
   fprintf(stdout, "\n");
   fprintf(stdout, "%lf\n", prodI);
   #endif

   //abre o arquivo para escrita binaria
   descritorArquivo = fopen(argv[2], "wb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }
   //escreve a dimensao
   ret1 = fwrite(&n, sizeof(long int), 1, descritorArquivo);
   if(ret1 < 1) {
    fprintf(stderr, "Erro de escrita no arquivo - dimensão\n");
    return 4;
 }


   //escreve os elementos do vetor A e do vetor B
   ret1 = fwrite(vetorA, sizeof(float), n, descritorArquivo);
   ret2 = fwrite(vetorB, sizeof(float), n, descritorArquivo);
   if((ret1 < n )&& (ret2 < n)) {
      fprintf(stderr, "Erro de escrita no arquivo - vetor A ou vetor B\n");
      return 4;
   }
   //escreve o produto interno
   ret1 = fwrite(&prodI, sizeof(double), 1, descritorArquivo);
   if(ret1 < 1) {
    fprintf(stderr, "Erro de escrita no arquivo - produto interno\n");
    return 4;
 }

   //finaliza o uso das variaveis
   fclose(descritorArquivo);
   free(vetorA);
   free(vetorB);
   return 0;
} 

