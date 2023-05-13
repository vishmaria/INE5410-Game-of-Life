#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "gol.h"
int n_threads;
//int global_i = 0;
game_t g;
// Função executada pelas worker threads. Definida em gol.c
stats_t play(game_t* g);

int main(int argc, char **argv)
{
    int size =  0, steps;
    cell_t **prev =  NULL, **next = NULL, **tmp = NULL;
    FILE *f;
    stats_t stats_step = {0, 0, 0, 0};
    stats_t stats_total = {0, 0, 0, 0};

    /* Iniciliza estrutura necessária */
    g.board = prev;
    g.newboard = next;
    g.size = size;

    if (argc != 3)
    {
        printf("ERRO! Você deve digitar %s <nome do arquivo do tabuleiro> <número de threads!>\n\n", argv[0]);
        return 0;
    }

    n_threads = atoi(argv[2]);
    /*  Inicialização threads */
    pthread_t threads[n_threads];

    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("ERRO! O arquivo de tabuleiro '%s' não existe!\n\n", argv[1]);
        return 0;
    }

    /* Cria tabuleiro e aloca celulas */
    fscanf(f, "%d %d", &size, &steps);

    g.board = allocate_board(g.size);
    g.newboard = allocate_board(g.size);
    read_file(f, g.board, g.size);

    fclose(f);

#ifdef DEBUG
    printf("Initial:\n");
    print_board(g.board, g.size);
    print_stats(stats_step);
#endif

    //while (global_i < steps) será utilizado na lógica
    for (int i = 0; i < steps; i++)
    {
        stats_step = play(&g);

        stats_total.borns += stats_step.borns;
        stats_total.survivals += stats_step.survivals;
        stats_total.loneliness += stats_step.loneliness;
        stats_total.overcrowding += stats_step.overcrowding;

#ifdef DEBUG
        printf("Step %d ----------\n", i + 1);
        print_board(g.newboard, g.size);
        print_stats(stats_step);
#endif
        tmp = g.newboard;
        g.newboard = g.board;
        g.board = tmp;
        for (size_t i = 0; i < n_threads; i++) {
            pthread_create(&threads[i], NULL, (void*(*)(void*)) play, &g);
}
    }

    for (int i = 0; i < n_threads; i++)
        pthread_join(threads[i], NULL);

#ifdef RESULT
    printf("Final:\n");
    print_board(g.board, g.size);
    print_stats(stats_total);
#endif

    free_board(g.board, g.size);
    free_board(g.newboard, g.size);
}
