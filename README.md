# INE5410-Game-of-Life
Trabalho 1 da disciplina INE5410 (programação concorrente), implementação de concorrência no game of life.
O Jogo da Vida (The Game of Life - GoL) é um autômato celular inventado pelo matemático John Conway da
Universidade de Cambridge (https://playgameoflife.com). Ele consiste em uma coleção de células que, baseadas
em algumas poucas regras matemáticas, podem viver, morrer ou se multiplicar. Dependendo das condições iniciais,
as células formam vários padrões durante todo o curso do jogo.

A versão do jogo adotada nesse trabalho possui um tabuleiro quadrado (matriz ou array 2D) onde as células são
atualizadas (modificadas) em cada geração (timestep) de acordo com as seguintes regras:
• Uma célula viva que possui menos de dois vizinhos vivos morre de solidão;
• Uma célula viva que possui dois ou três vizinhos vivos continua no mesmo estado para a próxima geração;
• Uma célula viva que possui mais de três vizinhos vivos morre de superpopulação;
• Uma célula morta que possui exatamente três vizinhos vivos se torna uma célula viva

# Compilação:
Para compilar, execute <b>make</b> no diretório principal do projeto. Para remover o binário, execute o comando <b>make
clean</b> a partir do diretório principal do projeto.

Após a compilação, você deverá executar o programa da seguinte maneira (exemplo de execução com o arquivo de
entrada chamado “input-little.in” armazenado no diretório principal do projeto):

./ gol input - little . in

Adicionalmente, você poderá combinar a saída do GoL com a ferramenta tr para gerar uma saída que permitirá
uma melhor visualização do resultado. A ideia é utilizar o tr para substituir os espaços em branco por “_”. Para isso,
utilize o seguinte comando:

./ gol input - little . in | tr ’ ’ ’_ ’

Por padrão, o programa somente irá imprimir na tela o último tabuleiro. Caso você deseje imprimir o tabuleiro ao
final de cada geração você precisará remover o comentário (#) da seguinte linha do arquivo Makefile: -DDEBUG.
Para medir o desempenho da sua solução paralela utilize a ferramenta time do Linux. Por exemplo, a seguinte
linha irá medir o tempo de execução do GoL utilizando como entrada o arquivo “input-big.in”. Você deverá observar
o tempo de execução na linha indicada por “real”.

time cat input - big . in | ./ gol

real 3 m1 .761 s

user 3 m1 .692 s

sys 0 m0 .160 s


Sempre que for medir o tempo evite fazer chamadas à função printf(). Para isso, deixe <b>#-DDEBUG</b> e <b>#-DRESULT</b> no
Makefile e recompile o programa.
