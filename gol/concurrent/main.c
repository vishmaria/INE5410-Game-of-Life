#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "gol.h"
/* Resolução: o tabuleiro foi dividido em pedaços menores para cada thread,
cada uma recebendo uma struct com os dados necessários como argumento de play().
Como temos um vetor dessas informações e cada thread executa seu pedaço de tabuleiro,
verificamos em atendimento com o professor que não é necessário 
nenhum mecanismo de controle adicional além da função join*/
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

    /* Caso o usuário faça besteira */
    int n_threads = atoi(argv[2]);
    if (!n_threads)
    {
        printf("VOCÊ DEVE INFORMAR PELO MENOS 1 THREAD!\n");
        return 1;
    }

    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("ERRO! O arquivo de tabuleiro '%s' não existe!\n\n", argv[1]);
        return 0;
    }

    /* Cria tabuleiro e aloca celulas */
    fscanf(f, "%d %d", &size, &steps);
    /*  Inicialização threads */
    pthread_t threads[n_threads];
    /* g deve ser um vetor para ter diferentes stats a cada iteração */
    game_t g[n_threads];
    int start = 0;
    /* Assumindo que temos um tabuleiro quadrado, dividimos suas posições
    pelo num. de threads. */
    int rst = (size * size) / n_threads;
    int r = (size * size) % n_threads;

    for (int i = 0; i < n_threads; i++)
    {
        int finish = start + rst;
        /* Se houver resto na divisão (n. impar) a thread irá percorrer um pedaço a mais. */
        if (i < r)
        {
            finish++;
        }
        /* Troca de chunks por thread */
        g[i].size = size;
        g[i].start = start;
        g[i].finish = finish;
        start = finish;
        
    }

    prev = allocate_board(size);
    next = allocate_board(size);
    read_file(f, prev, size);

    fclose(f);

#ifdef DEBUG
    printf("Initial:\n");
    print_board(prev, size);
    print_stats(stats_step);
#endif
    for (int i = 0; i < steps; i++)
    {
        /* Os stats são resetados a cada passo */
        stats_step.borns = 0;
        stats_step.loneliness = 0;
        stats_step.overcrowding = 0;
        stats_step.survivals = 0;
        for (int j = 0; j < n_threads; j++)
        {
            g[j].board = prev;
            g[j].newboard = next;
            g[j].stats = stats_step;
            pthread_create(&threads[j], NULL, play, (void *)&g[j]);
        }
        for (int j = 0; j < n_threads; j++)
        {
            /* Cálculo das estatísticas da geração após join 
            para não haver resultados incorretos. */
            pthread_join(threads[j], NULL);
            stats_step.borns += g[j].stats.borns;
            stats_step.survivals += g[j].stats.survivals;
            stats_step.loneliness += g[j].stats.loneliness;
            stats_step.overcrowding += g[j].stats.overcrowding;
        }

        /* Depois da execução das threads incrementamos o resultado final. */
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