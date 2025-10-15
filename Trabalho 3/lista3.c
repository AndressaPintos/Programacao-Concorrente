/*
Objetivo do programa: Usar a variação do padrão produtor/consumidor, tendo apenas um produtor, junto com sincronização
por semaforo. Cada consumidor deve veriicar se o número consumido é primo. Ao final deve ser contabilizado a quantidade de primos
e qual thread achou mais primo.
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>


#define N 100 //tamanho da sequencia de numeros
#define M 20 // tamanho do buffer
#define consumidores 4 // numero de consumidores

// Variaveis globais
sem_t slotCheio, bufferVazio;  // semaforos para sincronizacao por condicao
sem_t mutexGeral; //semaforo geral de sincronizacao por exclusao mutua (para gerar log)
sem_t terminouConsumir;// semaforo que indica que não há mais conteúdo para ser consumido

int Buffer[M],fim=0, processos[consumidores];//contabiliza o total de números processados por cada thread
int cont[consumidores];//contabiliza a quantidade de primos que cada thread encontrou


int ehPrimo(int n) {
    int i;
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
    for (i=3; i<sqrt(n)+1; i+=2)
    if(n%i==0) return 0;
    return 1;
}

//imprime o buffer
void printBuffer(int buffer[], int tam) {
  puts("\nBuffer:");
   for(int i=0;i<tam;i++) 
      printf("%d ", buffer[i]); 
   puts("");
}
  
//funcao para retirar  um elemento no buffer
int Retira (int id) {
   int item;
   static int out=0;
   static int max=0;
   sem_wait(&slotCheio); //aguarda slot cheio para retirar
   if(fim)
    return -1;
   sem_wait(&mutexGeral);//exclusao mutua entre consumidores
    item = Buffer[out];
    Buffer[out] = 0;
    out = (out + 1) % M;
    max++;
    printf("\nthread[%d]: retirou %d\n", id, item);
    printBuffer(Buffer, M); //para log apenas
    if(max==N){
      fim=1;
      sem_post(&terminouConsumir);//sinaliza não precisa mais de consumidores
    }
    if(out==0){
      sem_post(&bufferVazio); //sinaliza um slot vazio
      
    }
    sem_post(&mutexGeral);
    return item;
    
}

  // Consumidor
  void *consumidor(void * arg) {
    int item, id = *(int *)(arg);
    free(arg);
    while(1) {
      item = Retira(id); //retira o proximo item
      if(item==-1)
        pthread_exit(NULL);
      else if(ehPrimo(item)){
         cont[id]++;
      }
      processos[id]++;
    }
  }

//funcao para inserir um elemento no buffer
void Insere () {
    static int j = 0;
   sem_wait(&bufferVazio); //aguarda slot vazio para inserir
   sem_wait(&mutexGeral); //exclusao mutua entre produtores
   for(int i = 0;i<M && j<N;i++){
        Buffer[i] = j+1; 
        printf("Inserido: %d\n", j+1);
        j++;
        sem_post(&slotCheio); //sinaliza um slot cheio
    }
   printBuffer(Buffer, M); //para log apenas
   sem_post(&mutexGeral);
}


//será nosso fluxo produtor, pois só precisamos de um
int main(int argc, char **argv){
    int i, *id, totalPrimos=0, vencendora=0, valorVencedor=0;

    sem_init(&mutexGeral, 0, 1);
    sem_init(&slotCheio, 0, 0);
    sem_init(&bufferVazio, 0, 1);
    sem_init(&terminouConsumir, 0, 0);

    //criação das threads consumidoras
    pthread_t tid[consumidores];

    for(i=0;i<consumidores;i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        if (pthread_create(&tid[i], NULL, consumidor, (void *) (id))) {
        printf("Erro na criacao do thread produtor\n");
        exit(1);
        }
    }

    // inicio da produção
    int restante=N;
    while(restante>0){
        Insere();
        restante-=M;
    }
    
    //aguarda as consumidoras terminarem
    sem_wait(&terminouConsumir);

    //libera as consumidoras
    for(int i = 0; i < consumidores; i++) {
        sem_post(&slotCheio); 
     }

    //calculo da quantidade de primos e da thread vencedora
    
    for(int i=0;i<consumidores;i++){
      totalPrimos+=cont[i];
      vencendora = cont[i]>cont[vencendora] ? i : vencendora;
      valorVencedor = cont[i]>cont[vencendora] ? cont[i] : cont[vencendora];
      printf("\n--> thread[%d] processou %d numeros",i,processos[i]);
    }
    printf("\nTotal de primos encontrados: %d\nA thread vencedora é a %d com %d primos encontrados\n",totalPrimos,vencendora,valorVencedor);
    pthread_exit(NULL);
}