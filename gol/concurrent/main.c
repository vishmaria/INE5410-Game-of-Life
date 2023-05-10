#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "gol.h"
int n_threads;
// Função executada pelas worker threads. Definida em gol.c
stats_t play(void *arg);

int main(int argc, char **argv)
{
    int size, steps;
    cell_t **prev, **next, **tmp;
    FILE *f;
    stats_t stats_step = {0, 0, 0, 0};
    stats_t stats_total = {0, 0, 0, 0};

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

    prev = allocate_board(size);
    next = allocate_board(size);
    read_file(f, prev, size);

    fclose(f);
    /* Aloca dinamicamemte estrutura necessária */
        game* g = (game*) malloc(sizeof(game));
        g->board = prev;
        g->newboard = next;
        g->size = size;

#ifdef DEBUG
    printf("Initial:\n");
    print_board(prev, size);
    print_stats(stats_step);
#endif

    for (int i = 0; i < n_threads; i++)
    {
        stats_step = play(&g);

        stats_total.borns += stats_step.borns;
        stats_total.survivals += stats_step.survivals;
        stats_total.loneliness += stats_step.loneliness;
        stats_total.overcrowding += stats_step.overcrowding;

#ifdef DEBUG
        printf("Step %d ----------\n", i + 1);
        print_board(next, size);
        print_stats(stats_step);
#endif
        tmp = next;
        next = prev;
        prev = tmp;
        pthread_create(&threads[i], NULL, play, &g);
    }

    for (int i = 0; i < n_threads; i++)
        pthread_join(threads[i], NULL);

#ifdef RESULT
    printf("Final:\n");
    print_board(prev, size);
    print_stats(stats_total);
#endif

    free_board(prev, size);
    free_board(next, size);
}
