/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;


//classe runnable
class MyCallable implements Callable<Long> {

  private long n;

  // construtor
  MyCallable(long n) {
      this.n = n;     
  }

  // método para execução
  public Long call() throws Exception {
      if (n <= 1) {
          return 0L;
      }
      if (n == 2) {
          return 1L;
      }
      if (n % 2 == 0) {
          return 0L;
      }

      for (int i = 3; i <= Math.sqrt(n); i += 2) {
          if (n % i == 0) {
              return 0L;
          }
      }

      return 1L;
  }
}



//classe do método main
public class FutureMain{
  private static final int N = 100000;
  private static final int NTHREADS = 8;

  public static void main(String[] args) {
    //cria um pool de threads (NTHREADS)
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    //cria uma lista para armazenar referencias de chamadas assincronas
    List<Future<Long>> list = new ArrayList<Future<Long>>();

    for (int i = 0; i < N; i++) {
      Callable<Long> worker = new MyCallable(i);
      Future<Long> submit = executor.submit(worker);
      list.add(submit);
    }

    //recupera os resultados e faz o somatório final
    long sum = 0;
    for (Future<Long> future : list) {
      try {
        sum += future.get(); //bloqueia se a computação nao tiver terminado
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    System.out.println(sum);
    executor.shutdown();
  }
}