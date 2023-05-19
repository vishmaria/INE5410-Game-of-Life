#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "gol.h"

int n_threads;  // Número de threads
pthread_mutex_t born_mtx, surv_mtx, lon_mtx, over_mtx, mat_mtx; // Mutex usados

int main(int argc, char **argv) {
    int steps;  // Número de gerações do jogo
    args_t arg_play;  // Argumentos play
    cell_t **tmp;  // Matriz anterior, proxima, temporaria(atual)
    FILE *f;  // Arquivo
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

    fscanf(f, "%d %d", &arg_play.size, &steps);  // Lê informações do arquivo texto(tamanho matriz e número de gerações)

    // Atribui zero a todas estatisticas da geração atual
    arg_play.stats.borns = 0;
    arg_play.stats.loneliness = 0;
    arg_play.stats.overcrowding = 0;
    arg_play.stats.survivals = 0;

    // Aloca memoria para as matrizes usadas
    arg_play.board = allocate_board(arg_play.size);
    arg_play.newboard = allocate_board(arg_play.size);

    // Lê do arquivo e escreve a matriz em prev
    read_file(f, arg_play.board, arg_play.size);
    fclose(f);

    // Vetor com as threads
    pthread_t threads[n_threads];


#ifdef DEBUG
    printf("Initial:\n");
    print_board(arg_play.board, arg_play.size);
    print_stats(arg_play.stats);
#endif

    // Inicia os mutexes
    pthread_mutex_init(&born_mtx, NULL);
    pthread_mutex_init(&surv_mtx, NULL);
    pthread_mutex_init(&lon_mtx, NULL);
    pthread_mutex_init(&over_mtx, NULL);
    pthread_mutex_init(&mat_mtx, NULL);

    // Executa as 'steps' gerações
    for (int i = 0; i < steps; i++)
    {   
        // Atribui zero a todas estatisticas da geração atual
        arg_play.stats.borns = 0;
        arg_play.stats.loneliness = 0;
        arg_play.stats.overcrowding = 0;
        arg_play.stats.survivals = 0;

        // Criação das n_threads threads passando arg_play como argumento
        for (int t = 0; t < n_threads; t++) {
            pthread_create(&threads[t], NULL, play, &arg_play);
        }
        // Termino das threads
        for (int t = 0; t < n_threads; t++) {
            pthread_join(threads[t], NULL);
        }

        // Seta as estatisticas totais
        stats_total.borns += arg_play.stats.borns;
        stats_total.survivals += arg_play.stats.survivals;
        stats_total.loneliness += arg_play.stats.loneliness;
        stats_total.overcrowding += arg_play.stats.overcrowding;

#ifdef DEBUG
        printf("Step %d ----------\n", i + 1);
        print_board(arg_play.newboard, arg_play.size);
        print_stats(arg_play.stats);
#endif

        // reorganiza as matrizes com base na nova matriz da geração
        tmp = arg_play.newboard;
        arg_play.newboard = arg_play.board;
        arg_play.board = tmp;
    }

#ifdef RESULT
    printf("Final:\n");
    print_board(arg_play.board, arg_play.size);
    print_stats(stats_total);
#endif
    
    // Desaloca memória ocupada pelas matrizes
    free_board(arg_play.board, arg_play.size);
    free_board(arg_play.newboard, arg_play.size);

    // Destroi os mutexes
    pthread_mutex_destroy(&born_mtx);
    pthread_mutex_destroy(&surv_mtx);
    pthread_mutex_destroy(&lon_mtx);
    pthread_mutex_destroy(&over_mtx);
    pthread_mutex_destroy(&mat_mtx);
}
