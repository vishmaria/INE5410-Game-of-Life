#include <stdio.h>
#include "gol.h"

int main(int argc, char **argv)
{
    int size, steps;  // tamanho da matriz, números de gerações
    cell_t **prev, **next, **tmp;  // matriz anterior, proxima matriz, matriz temporaria
    FILE *f;  
    stats_t stats_step = {0, 0, 0, 0};  // estatisticas da geração atual
    stats_t stats_total = {0, 0, 0, 0};  // estatisticas totais de todas as gerações

    if (argc != 2)
    {
        printf("ERRO! Você deve digitar %s <nome do arquivo do tabuleiro>!\n\n", argv[0]);
        return 0;
    }

    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("ERRO! O arquivo de tabuleiro '%s' não existe!\n\n", argv[1]);
        return 0;
    }

    // Retira os dados do arquivo
    fscanf(f, "%d %d", &size, &steps);

    // Aloca as matrizes usadas
    prev = allocate_board(size);
    next = allocate_board(size);

    // Lê
    read_file(f, prev, size);
    fclose(f);

#ifdef DEBUG
    printf("Initial:\n");
    print_board(prev, size);
    print_stats(stats_step);
#endif

    for (int i = 0; i < steps; i++)
    {
        stats_step = play(prev, next, size);
        
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
    }

#ifdef RESULT
    printf("Final:\n");
    print_board(prev, size);
    print_stats(stats_total);
#endif

    free_board(prev, size);
    free_board(next, size);
}
