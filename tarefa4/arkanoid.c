/*******************************************/
/* Tarefa 4 de INF1805 - Sistemas Reativos */
/*                                         */
/*          Miguel Baldez Boing            */
/*******************************************/

#include <SDL2/SDL.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

/* Definicao da struct dos blocos a serem destruidos */

typedef struct tipo_bloco{
	SDL_Rect rec;
	int destroyed;
}Block;

/* Definicao de variaveis globais */

SDL_Rect ball = {140,420,10,10};
SDL_Rect bar = {270,470,100,10};
Block blocks[15];

/* Funcao de rederizacao */
void renderiza(SDL_Renderer* renderer){
	int i;

	/* Fundo */
	SDL_SetRenderDrawColor(renderer, 0x00,0x00,0x40,0x00);
    SDL_RenderFillRect(renderer, NULL);

	/* Blocos (tem a cor do fundo quando destruidos) */
	for(i=0;i<15;i++){
		if(blocks[i].destroyed)
			SDL_SetRenderDrawColor(renderer, 0x00,0x00,0x40,0x00);
		else if(i%2)
			SDL_SetRenderDrawColor(renderer, 0xFF,0x66,0x00,0x00);
		else
			SDL_SetRenderDrawColor(renderer, 0xFF,0x99,0x00,0x00);
       	SDL_RenderFillRect(renderer, &blocks[i].rec);
	}

	/* Barra */
	SDL_SetRenderDrawColor(renderer, 0xFF,0xFF,0xFF,0x00);
    SDL_RenderFillRect(renderer, &bar);

	/* Bola (vermelha quando toca no chao) */
	if(ball.y == 470)
		SDL_SetRenderDrawColor(renderer, 0xFF,0x00,0x00,0x00);
	else
		SDL_SetRenderDrawColor(renderer, 0xFF,0xFF,0xFF,0x00);
    SDL_RenderFillRect(renderer, &ball);

	SDL_RenderPresent(renderer);
}

int main (int argc, char* args[])
{
    /* Inicializacao do SDL */

    int err = SDL_Init(SDL_INIT_EVERYTHING);
    assert(err == 0);

    SDL_Window* window = SDL_CreateWindow("Arkanoid",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            640, 480, SDL_WINDOW_SHOWN
                         );
    assert(window != NULL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    assert(renderer != NULL);

    /* EXECUCAO */

	SDL_Event e;
	int xspeed = 10;
	int yspeed = 10;
	int blocksDestroyed = 0;
	int i;
	int randnum;
	Uint32 t;
	Uint32 ultimot = 0;

	/* Criacao e posicionamento dos 15 blocos */

	for(i=0;i<15;i++){
		blocks[i].rec.x = 70 + 100*(i%5);
		blocks[i].rec.y = 80 + 40*(i/5);
		blocks[i].rec.w = 100;
		blocks[i].rec.h = 40;
		blocks[i].destroyed = 0;
	}

	/* Criacao de um numero aleatorio para que o jogo inicie em uma posicao arbitraria */
	
	srand(time(NULL));
	randnum = (rand()%55)*10;
	ball.x = randnum;	
	bar.x = randnum;

	/* Loop principal do jogo */

	while(1){
		SDL_PollEvent(&e);

		t = (int) SDL_GetTicks();

		/* Terminar o jogo caso o usuario feche a janela */

		if (e.type == SDL_QUIT) {
            break;
		}

		/* Leitura do teclado para fazer o movimento da barra */

		else if(e.type == SDL_KEYDOWN){
			switch (e.key.keysym.sym){
				case SDLK_LEFT:
					if(bar.x > 0) /* Para nao sair da tela */
						bar.x -= 2 + 0.1*(blocksDestroyed);
					break;
				case SDLK_RIGHT:
					if(bar.x < 540) /* Para nao sair da tela */
						bar.x += 2 + 0.1*(blocksDestroyed);
					break;
			}
		}

		/* A cada periodo de tempo a bola se move e busca as colisoes, este periodo diminui com o passar do jogo */

		if(t - ultimot >= 50 - 2.5*(blocksDestroyed)){

			ultimot = t;

			/* Movimento da bola */
			ball.x += xspeed;
			ball.y += yspeed;

			/* Teste de colisao com as paredes */
			if(ball.x == 0 || ball.x == 630)
				xspeed = -xspeed;

			/* Teste de colisao com o teto */
			if(ball.y == 0)
				yspeed = -yspeed;

			/* Teste de colisao com a barra */
			if(ball.y == 460 && ball.x >= bar.x && ball.x <= (bar.x + bar.w))
				yspeed = -yspeed;

			/* Teste de colisao com o chao */
			if(ball.y == 470){
				/* Chama a renderizacao para a bola ficar vermelha */
				renderiza(renderer);

				/* Mensagem de fim de jogo */
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Game Over","PERDESTE!",NULL);
				break;
			}

			/* Teste de colisao com cada um dos blocos */
			for(i=0;i<15;i++){
				if(!blocks[i].destroyed){

					/* Com a parte esquerda do bloco */
					if((ball.x + ball.w) == blocks[i].rec.x
						&& ball.y >= blocks[i].rec.y
						&& (ball.y + ball.h) <= (blocks[i].rec.y + blocks[i].rec.h)){
							xspeed = -xspeed;
							blocks[i].destroyed = 1;
							blocksDestroyed++;
					}

					/* Com a parte direita do bloco */			
					else if(ball.x == (blocks[i].rec.x + blocks[i].rec.w)
						&& ball.y >= blocks[i].rec.y
						&& (ball.y + ball.h) <= (blocks[i].rec.y + blocks[i].rec.h)){
							xspeed = -xspeed;
							blocks[i].destroyed = 1;
							blocksDestroyed++;
					}

					/* Com a parte superior do bloco */
					else if((ball.y + ball.h) == blocks[i].rec.y
						&& ball.x >= blocks[i].rec.x
						&& (ball.x + ball.w) <= (blocks[i].rec.x + blocks[i].rec.w)

						/* Com as duas quinas superiores do bloco */
						|| ball.x == blocks[i].rec.x && ball.y == blocks[i].rec.y
						|| ball.x == (blocks[i].rec.x + 90) && ball.y == blocks[i].rec.y){
							yspeed = -yspeed;
							blocks[i].destroyed = 1;
							blocksDestroyed++;
					}

					/* Com a parte inferior do bloco */			
					else if(ball.y == (blocks[i].rec.y + blocks[i].rec.h)
						&& ball.x >= blocks[i].rec.x
						&& (ball.x + ball.w) <= (blocks[i].rec.x + blocks[i].rec.w)

						/* Com as duas quinas inferiores do bloco */
						|| ball.x == blocks[i].rec.x && ball.y == (blocks[i].rec.y + 30)
						|| ball.x == (blocks[i].rec.x + 90) && ball.y == (blocks[i].rec.y + 30)){
							yspeed = -yspeed;
							blocks[i].destroyed = 1;
							blocksDestroyed++;
					}
				}
			}
		}

		/* Chamada da funcao de renderizacao */
		renderiza(renderer);

		/* Testa se todos os blocos foram destruidos para poder terminar o jogo */		
		if(blocksDestroyed == 15){
			/* Mensagem de fim de jogo */
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Game Over","GANHASTE!",NULL);
			break;
		}
	}

    /* FINALIZACAO */

    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}
