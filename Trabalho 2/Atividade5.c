//Objetivo do programa: coordenar as threads ExecutaTarefa e a thread Extra por meio de sincronização por variável de condição.
//De forma que as threads ExecutaTarefa pausam a soma ao chega num múltiplo de 1000 e só proxigam após a thread extra printar o múltiplo 

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

//variaveis globais
long int soma = 0; //variavel compartilhada entre as threads
pthread_mutex_t mutex; //variavel de lock para exclusao mutua
pthread_cond_t condTarefa;
pthread_cond_t condExtra; //variavel de condição para sincronização
int nthreads; //qtde de threads (passada linha de comando)
int sinal=0;//Usado para que somente uma thread avise que chegou em um múltiplo de 1000, impedindo as demais de entrarem no if

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  printf("Thread : %ld esta executando...\n", id);
  
  for (int i=0; i<100000; i++) {
    pthread_mutex_lock(&mutex);
    while (sinal){
      pthread_cond_wait(&condTarefa, &mutex);//outras threads aguardam o sinal da thread extra para continuarem
    }
    soma++;
    if (!(soma%1000)){
      sinal=1;
      pthread_cond_signal(&condExtra);//libera thread extra para printa
      pthread_cond_wait(&condTarefa, &mutex);//aguarda até a thread extra printar
      while(sinal){
        pthread_cond_wait(&condTarefa, &mutex);//thread atual fica restestando o sinal
      }
    }
    pthread_mutex_unlock(&mutex);
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

///funcao executada pela thread de log
void *extra (void *args) {
  printf("Extra : esta executando...\n");
  int somaVelho=1;//variável auxiliar
  for(int i=0; i<nthreads*100;){
    pthread_mutex_lock(&mutex);
    while (!sinal){
      pthread_cond_wait(&condExtra, &mutex);//só prossegue quando a thread tarefa encontra um múltiplo de 1000
    }
    if(somaVelho-soma){
      printf("%dº múltiplo = %ld \n", i+1,soma); //evita contabilização repetida
      i++;
    }
    sinal=0;//avisa que as threads de tarefa podem voltar a executar
    pthread_cond_broadcast(&condTarefa);//libera as threads para continuar a soma
    somaVelho=soma;
    pthread_mutex_unlock(&mutex);
  }
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}


//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   //--le e avalia os parametros de entrada
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex e o cond
   pthread_mutex_init(&mutex, NULL);
   pthread_cond_init(&condTarefa, NULL);
   pthread_cond_init(&condExtra, NULL);

   //--cria as threads
   for(long int t=0; t<nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--cria thread de log
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--espera todas as threads terminarem
   for (int t=0; t<nthreads+1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   } 

   //--finaliza o mutex
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&condExtra);
   pthread_cond_destroy(&condTarefa);
   
   printf("Valor de 'soma' = %ld\n", soma);

   return 0;
}