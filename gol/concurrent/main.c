#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "gol.h"

/*Variaveis globais*/
pthread_mutex_t born_mtx, surv_mtx, lon_mtx, over_mtx, mat_mtx; // Mutex usados
int linha_atual, coluna_atual;  // Usada para setar as posições da matriz em cada thread
cell_t **next, **prev;  // Matrizes atuais e novas
int size;  // Tamanho da matriz


int main(int argc, char **argv) {
    int n_threads;  // Número de threads
    int steps;  // Número de gerações do jogo
    cell_t **tmp;  // Matriz temporaria(auxilia em cada geração)
    FILE *f;  // Arquivo de entrada
    stats_t stats_total = {0, 0, 0, 0};  // Estatisticas de todas as gerações
    stats_t stats_step;  // Estatisticas da geração atual

    // Se número de argumentos errados
    if (argc != 3) {
        printf("ERRO! Você deve digitar %s <nome do arquivo do tabuleiro> <número de threads!>\n\n", argv[0]);
        return 0;
    }

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

    n_threads = atoi(argv[2]); // Converte para inteiro o número de threads passadas por argumento
    pthread_t threads[n_threads]; // Vetor para as threads usadas

#ifdef DEBUG
    printf("Initial:\n");
    print_board(prev, size);
    print_stats(stats_total);
#endif

    // Inicia os mutexes
    pthread_mutex_init(&born_mtx, NULL);
    pthread_mutex_init(&surv_mtx, NULL);
    pthread_mutex_init(&lon_mtx, NULL);
    pthread_mutex_init(&over_mtx, NULL);
    pthread_mutex_init(&mat_mtx, NULL);

    // Executa as 'steps' gerações
    for (int i = 0; i < steps; i++) {
        
        // Nova geração
        linha_atual = 0;
        coluna_atual = 0;
        stats_step.survivals = 0;
        stats_step.loneliness = 0;
        stats_step.overcrowding = 0;
        stats_step.borns = 0;

        // Criação das n_threads threads passando stats_step como argumento
        for (int t = 0; t < n_threads; t++) {
            pthread_create(&threads[t], NULL, play, &stats_step);
        }
        // Termino das threads
        for (int t = 0; t < n_threads; t++) {
            pthread_join(threads[t], NULL);
        }

        // Seta as estatisticas totais
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

    }

#ifdef RESULT
    printf("Final:\n");
    print_board(prev, size);
    print_stats(stats_total);
#endif
    
    // Desaloca memória ocupada pelas matrizes
    free_board(prev, size);
    free_board(next, size);

    // Destroi os mutexes
    pthread_mutex_destroy(&born_mtx);
    pthread_mutex_destroy(&surv_mtx);
    pthread_mutex_destroy(&lon_mtx);
    pthread_mutex_destroy(&over_mtx);
    pthread_mutex_destroy(&mat_mtx);
}
