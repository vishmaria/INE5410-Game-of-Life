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
stats_t stats;

/* Variáveis globais */
extern pthread_mutex_t born_mtx, surv_mtx, lon_mtx, over_mtx, mat_mtx; // Mutex usados

// Usados para setar linha e coluna de cada thread
extern int linha_atual, coluna_atual;


//extern int n_threads;   //Define variável declarada em main.c

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
    args_t* args = (args_t*) arg;
    int minha_linha, minha_coluna, a; 

    // Enquanto não percorrer todos elementos da matriz
    while (linha_atual < args->size) {
        
        // Região critica: seta as linhas e colunas que as threads vão operar até finalizar
        pthread_mutex_lock(&mat_mtx);
        minha_linha = linha_atual;
        minha_coluna = coluna_atual;

        coluna_atual += 1;
        if (coluna_atual >= args->size) {  // Quando chegar no fim da coluna
            coluna_atual = 0;  // Primeiro elemento 
            linha_atual += 1;  // Da próxima linha
        }   
        pthread_mutex_unlock(&mat_mtx);
        // Fim da região critica

        // Se acabar as linhas da matriz
        if (minha_linha >= args->size) {
            break;
        }

        a = adjacent_to(args->board, args->size, minha_linha, minha_coluna);  // Quantas celulas adjacentes a coordenada tem

        /* if cell is alive */
        if(args->board[minha_linha][minha_coluna]) 
        {
            /* death: loneliness */
            if(a < 2) {
                args->newboard[minha_linha][minha_coluna] = 0;
                pthread_mutex_lock(&lon_mtx);
                args->stats.loneliness++;
                pthread_mutex_unlock(&lon_mtx);
            }
            else
            {
                /* survival */
                if(a == 2 || a == 3)
                {
                    args->newboard[minha_linha][minha_coluna] = args->board[minha_linha][minha_coluna];
                    pthread_mutex_lock(&surv_mtx);
                    args->stats.survivals++;
                    pthread_mutex_unlock(&surv_mtx);
                }
                else
                {
                    /* death: overcrowding */
                    if(a > 3)
                    {
                        args->newboard[minha_linha][minha_coluna] = 0;
                        pthread_mutex_lock(&over_mtx);
                        args->stats.overcrowding++;
                        pthread_mutex_unlock(&over_mtx);
                    }
                }
            }
            
        }
        else /* if cell is dead */
        {
            if(a == 3) /* new born */
            {
                args->newboard[minha_linha][minha_coluna] = 1;
                pthread_mutex_lock(&born_mtx);
                args->stats.borns++;
                pthread_mutex_unlock(&born_mtx);
            }
            else /* stay unchanged */
                args->newboard[minha_linha][minha_coluna] = args->board[minha_linha][minha_coluna];
        }
    }
    pthread_exit(NULL);
    return 0;
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