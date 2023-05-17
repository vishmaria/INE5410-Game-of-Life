#include <stdio.h>
#include <pthread.h>
#include "gol.h"

int n_threads;  // Número de threads

int main(int argc, char **argv) {
    int size, steps;  // Tamanho matriz, número de gerações do jogo
    cell_t **prev, **next, **tmp;  // Matriz anterior, proxima, temporaria(atual)
    FILE *f;
    stats_t stats_step = {0, 0, 0, 0};  // Estatisticas de cada geração
    stats_t stats_total = {0, 0, 0, 0};  // Estatisticas de todas as gerações

    // Se número de argumentos errados
    if (argc != 3) {
        printf("ERRO! Você deve digitar %s <nome do arquivo do tabuleiro> <número de threads!>\n\n", argv[0]);
        return 0;
    }

    n_threads = atoi(argv[2]);  // Converte o argumento de número de threads para inteiro

    // Se nome do arquivo tabuleiro errado
    if ((f = fopen(argv[1], "r")) == NULL) {
        printf("ERRO! O arquivo de tabuleiro '%s' não existe!\n\n", argv[1]);
        return 0;
    }

    fscanf(f, "%d %d", &size, &steps);  // Lê informações do arquivo texto(tamanho matriz e número de gerações)

    // Aloca memoria para as matrizes usadas
    prev = allocate_board(size);
    next = allocate_board(size);

    // Lê do arquivo e escreve a matriz em prev
    read_file(f, prev, size);
    fclose(f);

    /*  Inicialização threads */
    pthread_t threads[n_threads];


#ifdef DEBUG
    printf("Initial:\n");
    print_board(prev, size);
    print_stats(stats_step);
#endif

    // Executa as 'steps' gerações
    for (int i = 0; i < size; i++)
    {   
        stats_step = play(prev, next, size);  // Começa uma geração
        
        // Variaveis globais, CUIDADO!!!
        stats_total.borns += stats_step.borns;
        stats_total.survivals += stats_step.survivals;
        stats_total.loneliness += stats_step.loneliness;
        stats_total.overcrowding += stats_step.overcrowding;

#ifdef DEBUG
        printf("Step %d ----------\n", i + 1);
        print_board(next, size);
        print_stats(stats_step);
#endif

        // reorganiza as matrizes com base na nova matriz da geração
        tmp = next;
        next = prev;
        prev = tmp;

#ifdef RESULT
    printf("Final:\n");
    print_board(prev, size);
    print_stats(stats_total);
#endif
    
    // Desaloca memória ocupada pelas matrizes
    free_board(prev, size);
    free_board(next, size);
}
