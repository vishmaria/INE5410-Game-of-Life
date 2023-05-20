/*
 * The Game of Life
 *
 * RULES:
 *  1. A cell is born, if it has exactly three neighbours.
 *  2. A cell dies of loneliness, if it has less than two neighbours.
 *  3. A cell dies of overcrowding, if it has more than three neighbours.
 *  4. A cell survives to the next generation, if it does not die of lonelines or overcrowding.
 *
 * In this version, a 2D array of ints is used.  A 1 cell is on, a 0 cell is off.
 * The game plays a number of steps (given by the input), printing to the screen each time.
 * A 'x' printed means on, space means off.
 *
 */

#include <stdlib.h>
#include "gol.h"
#include <stdlib.h>
#include <pthread.h>

/* Statistics */
stats_t statistics;

/* Variáveis globais */
extern pthread_mutex_t born_mtx, surv_mtx, lon_mtx, over_mtx; // Mutex usados
extern int linha_atual, coluna_atual; // Auxilia na definição de posição da matriz
extern cell_t **next, **prev;  // Matrizes atuais e novas
extern int size; // Tamanho da matriz
extern stats_t stats_step; // Estatisticas da geração atual
extern int n_threads;  // Numero de threads

// Divide as linhas que cada thread opera
d_thread_t divide_matriz(int index) {
    d_thread_t dado;
    int div = size/n_threads;
    int res = size%n_threads;
    dado.inicio = div * index;
    if (index == n_threads-1) {
        dado.fim = (div * (index+1))+res-1;
    } else {
        dado.fim = (div * (index+1))-1;
    }
    return dado;
}


cell_t **allocate_board(int size)
{
    cell_t **board = (cell_t **)malloc(sizeof(cell_t *) * size);
    int i;
    for (i = 0; i < size; i++)
        board[i] = (cell_t *)malloc(sizeof(cell_t) * size);
    
    statistics.borns = 0;
    statistics.survivals = 0;
    statistics.loneliness = 0;
    statistics.overcrowding = 0;

    return board;
}

void free_board(cell_t **board, int size)
{
    int i;
    for (i = 0; i < size; i++)
        free(board[i]);
    free(board);
}

/* Retorna quantas celulas são adjacentes a i, j, celula */
int adjacent_to(cell_t **board, int size, int i, int j)
{
    int k, l, count = 0;

    int sk = (i > 0) ? i - 1 : i;
    int ek = (i + 1 < size) ? i + 1 : i;
    int sl = (j > 0) ? j - 1 : j;
    int el = (j + 1 < size) ? j + 1 : j;

    for (k = sk; k <= ek; k++)
        for (l = sl; l <= el; l++)
            count += board[k][l];
    count -= board[i][j];

    return count;
}

void* play(void * arg)
{   
    d_thread_t dado = *(d_thread_t*) arg;
    int minha_linha, minha_coluna, a;

    // Percorre as linhas especificas de cada thread
    for (minha_linha = dado.inicio; minha_linha <= dado.fim; minha_linha++) {
        for (minha_coluna = 0; minha_coluna < size; minha_coluna++) {
            a = adjacent_to(prev, size, minha_linha, minha_coluna);  // Quantas celulas adjacentes a coordenada tem

            /* if cell is alive */
            if(prev[minha_linha][minha_coluna]) 
            {
                /* death: loneliness */
                if(a < 2) {
                    next[minha_linha][minha_coluna] = 0;
                    pthread_mutex_lock(&lon_mtx);
                    stats_step.loneliness++;
                    pthread_mutex_unlock(&lon_mtx);
                }
                else
                {
                    /* survival */
                    if(a == 2 || a == 3)
                    {
                        next[minha_linha][minha_coluna] = prev[minha_linha][minha_coluna];
                        pthread_mutex_lock(&surv_mtx);
                        stats_step.survivals++;
                        pthread_mutex_unlock(&surv_mtx);
                    }
                    else
                    {
                        /* death: overcrowding */
                        if(a > 3)
                        {
                            next[minha_linha][minha_coluna] = 0;
                            pthread_mutex_lock(&over_mtx);
                            stats_step.overcrowding++;
                            pthread_mutex_unlock(&over_mtx);
                        }
                    }
                }
                
            }
            else /* if cell is dead */
            {
                if(a == 3) /* new born */
                {
                    next[minha_linha][minha_coluna] = 1;
                    pthread_mutex_lock(&born_mtx);
                    stats_step.borns++;
                    pthread_mutex_unlock(&born_mtx);
                }
                else /* stay unchanged */
                    next[minha_linha][minha_coluna] = prev[minha_linha][minha_coluna];
            }
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void print_board(cell_t **board, int size)
{
    int i, j;
    /* for each row */
    for (j = 0; j < size; j++)
    {
        /* print each column position... */
        for (i = 0; i < size; i++)
            printf("%c", board[i][j] ? 'x' : ' ');
        /* followed by a carriage return */
        printf("\n");
    }
}

void print_stats(stats_t stats)
{
    /* print final statistics */
    printf("Statistics:\n\tBorns..............: %u\n\tSurvivals..........: %u\n\tLoneliness deaths..: %u\n\tOvercrowding deaths: %u\n\n",
        stats.borns, stats.survivals, stats.loneliness, stats.overcrowding);
}

void read_file(FILE *f, cell_t **board, int size)
{
    char *s = (char *) malloc(size + 10);

    /* read the first new line (it will be ignored) */
    fgets(s, size + 10, f);

    /* read the life board */
    for (int j = 0; j < size; j++)
    {
        /* get a string */
        fgets(s, size + 10, f);

        /* copy the string to the life board */
        for (int i = 0; i < size; i++)
            board[i][j] = (s[i] == 'x');
    }

    free(s);
}