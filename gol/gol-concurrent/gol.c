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
#include <math.h>

/* Statistics */
stats_t statistics;

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

/* int trunc_division(int x, int y)
{
    int rst = x / y;
    int r = x % y;
    if (r < y / 2)
    {
        return rst;
    }
    else
        return (rst + 1);
}
 */
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

/* per_threads *split_board()
{
    per_threads *threads = malloc(sizeof(per_threads));
    Vamos dividir o tabuleiro entre as threads
    threads->rows = n_threads;
    threads->cols = 1;
     Aqui reduzimos o número de iterações para encontrar um produto i*j
    igual a n_threads. Se um deles é maior que a raiz de n_threads, o produto será maior
    e esse valor não é interessante.
    for (int i = 1; i <= sqrt(n_threads); i++)
    {
        for (int j = sqrt(n_threads); j <= n_threads; j++)
        {
            Queremos i e j com menor diferença possível para um bloco mais quadrado.
            Para que a subtração não seja negativa usa-se abs()
            if (i * j == n_threads && abs(i - j) < abs(threads->rows - threads->cols))
            {
                threads->rows = i;
                threads->cols = j;
            }
        }
        Cada thread terá size/rows linhas e size/cols colunas,
        mas é preiciso tratar divisão não inteira.
        threads->rows = trunc_division(g->size, threads->rows);
        threads->cols = trunc_division(g->size, threads->cols);
    }
    return threads;
}*/

void *play(void *arg)
{
    game_t *g = (game_t *)arg;
    int x, a;
    for (x = g->start; x < g->finish; x++)
    {
        /* Cálculo da linha i e coluna j correspondente a x.
        a divisão inteira de x por size determina quantas linhas
        foram percorridas. 
        O resto dessa divisão é a posição dentro desse i, i.e, a coluna. */
        int i = x / g->size;
        int j = x % g->size;
        a = adjacent_to(g->board, g->size, i, j);
        /* if cell is alive */
        if (g->board[i][j])
        {
            /* death: loneliness */
            if (a < 2)
            {
                g->newboard[i][j] = 0;
                g->stats.loneliness++;
            }
            else
            {
                /* survival */
                if (a == 2 || a == 3)
                {
                    g->newboard[i][j] = g->board[i][j];
                    g->stats.survivals++;
                }
                else
                {
                    /* death: overcrowding */
                    if (a > 3)
                    {
                        g->newboard[i][j] = 0;
                        g->stats.overcrowding++;
                    }
                }
            }
        }
        else /* if cell is dead */
        {
            if (a == 3) /* new born */
            {
                g->newboard[i][j] = 1;
                g->stats.borns++;
            }
            else /* stay unchanged */
                g->newboard[i][j] = g->board[i][j];
        }
    }
    pthread_exit(NULL);
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
    char *s = (char *)malloc(size + 10);

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