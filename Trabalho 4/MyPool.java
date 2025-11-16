/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Criando um pool de threads em Java */

import java.util.LinkedList;

//-------------------------------------------------------------------------------

/*
 * Os métidos dessa classe permitem criar a pool threads, adicionar novas tarefas e fechar a piscina
 */
class FilaTarefas {
    //variáveis
    private final int nThreads; //número de threads
    private final MyPoolThreads[] threads;// lista das threads que estão na piscina
    private final LinkedList<Runnable> queue;// lista de tarefas
    private boolean shutdown;// sinalizador binário usado para fechar a lista de tarefas

    //Adiciona cada thread a piscina
    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();// cria uma lista vazia
        threads = new MyPoolThreads[nThreads];//cria as threads e as adiciona a uma lista
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    //Usa sincrionização por exclusão mútua para adicionar uma tarefa a fila
    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;//shutdown é o sinalizador que indica se a piscina ainda está recebendo tarefas
            queue.addLast(r);
            queue.notify();//pq não notifyall?
        }
    }
    
    //encerra a entrada de novas tarefas na piscina e aguarda o retorno da threads
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    /*
     *?
     */
    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty()) return;   
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

//--PASSO 1: cria uma classe que implementa a interface Runnable 
/*
class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   //--metodo executado pela thread
   public void run() {
      System.out.println(msg);
   }
}
*/
class Primo implements Runnable {
    int n;
    public Primo(int valor) {
        n = valor;
    }

    public void run() {
        String msg;
    
        if (n <= 1) {
            msg = " não é";
        } else if (n == 2) {
            msg = " é";
        } else if (n % 2 == 0) {
            msg = " não é";
        } else {
            msg = " é";
            for (int i = 3; i <= Math.sqrt(n); i += 2) {
                if (n % i == 0) {
                    msg = " não é";
                    break;
                }
            }
        }
    
        System.out.println(n+msg+" primo");
    }
}    
//Classe da aplicação (método main)
class MyPool {
    private static final int NTHREADS = 8;
    private static final int N = 100000;

    public static void main (String[] args) {
      //--PASSO 2: cria o pool de threads
      FilaTarefas pool = new FilaTarefas(NTHREADS); 
      
      //--PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
      for (int i = 1; i <= N; i++) {
        //final String m = "Hello da tarefa " + i;
        //Runnable hello = new Hello(m);
        //pool.execute(hello);
        Runnable primo = new Primo(i);
        pool.execute(primo);
      }

      //--PASSO 4: esperar pelo termino das threads
      pool.shutdown();
   }
}