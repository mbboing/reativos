Tarefa 5 de INF1805 - Sistemas Reativos
Miguel Baldez Boing

Variaveis de estado utilizadas em arkanoid.c(versao da tarefa 5):

int blocksDestroyed:
Contador de quantos blocos ja foram destruidos na partida atual.
Usado para aumentar a velocidade do jogo e testar se o jogador ganhou, caso o valor seja 15.

O campo int destroyed da struct Block:
Possui o valor 1 se o bloco em questao ja foi destruido e 0 caso contrario.
Usado para saber se a colisao com a bola deve ser testada ou nao e para ver como fazer a renderizacao.

Uint32 t e Uint32 ultimot:
Armazena quanto tempo ja se passou e qual foi o tempo na ultima passada pelo if do tempo.
Usadas para controlar de quanto em quanto tempo deve-se executar os movimentos, testes de colisoes e renderizacoes.
