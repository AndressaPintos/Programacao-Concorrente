import subprocess
import sys
import math


# Função: verifica se um número é primo de forma sequencial
def ehPrimo(n):
    if n <= 1:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    r = int(math.sqrt(n))
    for i in range(3, r + 1, 2):
        if n % i == 0:
            return False
    return True


# Função: executa executa o programa java que lista a quantidade de primo
def run_java(program):
    try:
        result = subprocess.run(
            ["java", program],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        return result.stdout.strip()
    except FileNotFoundError:
        print("Erro: Java não encontrado.")
        sys.exit(1)



def main():
    N = 100000
    
    print(f"Testando com N = {N}\n")# é preciso mudar nas funções

    # PRIMEIRO PROGRAMA: Lista primos e não primos
    java_output_1 = run_java("MyPool")
    
    linhasJava = sorted(java_output_1.split("\n"))
    linhasPython = sorted(
        f"{i} é primo" if ehPrimo(i) else f"{i} não é primo"
        for i in range(1, N + 1)
    )

    for i in range(1, N + 1):
        if ehPrimo(i):
            linhasPython.append(f"{i} é primo")
        else:
            linhasPython.append(f"{i} não é primo")

    print("Testando MyPool...")
    if linhasJava == linhasPython:
        print("MyPool está CORRETO\n")
    else:
        print("MyPool está INCORRETO")
        print("Primeiras diferenças encontradas:\n")
        for i, (jLinha, pLinha) in enumerate(zip(linhasJava, linhasPython), start=1):
            if ((jLinha != pLinha)):
                print(f"Linha {i}:")
                print(f"Java:   {jLinha}")
                print(f"Python: {pLinha}")
                break
        print()

    # SEGUNDO PROGRAMA: Contagem de primos até N
    java_output_2 = run_java("FutureMain")
    contadorJava = int(java_output_2.strip())

    contadorPython = sum(1 for i in range(1, N + 1) if ehPrimo(i))

    print("Testando FutureMain...")
    if contadorJava == contadorPython:
        print("FutureMain está CORRETO")
    else:
        print("FutureMain está INCORRETO")
        print(f"Java  : {contadorJava}")
        print(f"Python: {contadorPython}")


if __name__ == "__main__":
    main()
