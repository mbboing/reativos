/*
Autores:
Bernard Jean Lupiac	1311792
Miguel Baldez Boing	1312013
*/

#include <stdio.h>
#include <SDL2/SDL.h>
#include <assert.h>
#include <math.h>

#define PX_PER_MM	3.779

/* Struct que armazena todas as informacoes
de um sensor de rede sem fio*/
typedef struct sensor {
    int x;
    int y;
    int info;
    int nodeID;
} Sensor;

/* Struct que representa uma celula
da matriz que representa a sala*/
typedef struct cell {
    SDL_Rect r;
    int* sens_dist;
} Cell;

/* Funcao que periodicamente checa a existencia
de um novo arquivo e atuliza os dados*/
int getval(Sensor* sens, int numsens, int batch) {
    int i;
    FILE* input;
    char filename[30];
    
	/* so se quer ler o arquivo com o indice "batch" dado */
	sprintf(filename, "Data/batch%d.txt", batch);

    input = fopen(filename, "r");
    if(input == NULL) {
		#ifdef _DEBUG
        printf("Access to file denied.");
		#endif
        return 0;
    }

    for(i = 0; i < numsens; i++)
        fscanf(input, " %d", &(sens[i].info));
    
	fclose(input);

    return i;
}

/* Funcao que inicializa os valores do programa
a partir de um arquivo modificado pelo usuario*/
int getconf(Sensor** sens, int* room_x, int* room_y, int* res, float* scale,
			int* val_min, int* val_max, int* numsens, int* factor, char *mode) {
    int i;
	char dummy;
    FILE* input;

    input = fopen("config.txt", "r");
    if(input == NULL) {
		#ifdef _DEBUG
        printf("Access to file denied.");
		#endif
		return 0;
    }
    
	/* atribui valores contantes*/
	fscanf(input, " %d %c %d %d %d %f %d %d %d",
		   numsens, mode, room_x, room_y, res, scale, val_min, val_max, factor);

	*room_x *= PX_PER_MM*(*scale);
	*room_y *= PX_PER_MM*(*scale);

	/* aloca vetor de sensores baseando-se no numero de
	sensores sem fio dado pelo usuario no arquivo lido*/
	*sens = (Sensor*) malloc(sizeof(Sensor) * *numsens);

	/* inicializa todos os sensores*/
    for(i = 0; i < *numsens; i++) {
        fscanf(input, " %d %d %d %d", &((*sens)[i].x), &((*sens)[i].y),
		       &((*sens)[i].info), &((*sens)[i].nodeID));
		
		(*sens)[i].x *= PX_PER_MM*(*scale);
		(*sens)[i].y *= PX_PER_MM*(*scale);

	}

	fclose(input);
    
    return i;
}



int main(void) {
    
    /* INITIALIZATION */
    
    int err = SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;    
    SDL_Renderer* renderer;
    
    /* EXECUTION */
    int i, j, k, batch = 0;
	int room_x, room_y, res, val_min, val_max, numsens, factor;
    float color, temp, tempDen, tempNum, tempMax, tempMin, scale;
	char mode;
    Sensor* sens = NULL;
    Cell** room;
    SDL_Event e;

	/*Inicializa o programa em lua que recebe os dados dos sensores e escreve nos arquivos*/
	system("lua receiveData.lua &");
    
	/* inicializando os valores */
	getconf(&sens, &room_x, &room_y, &res, &scale, &val_min, &val_max, &numsens, &factor, &mode);

	window = SDL_CreateWindow   ("Room Sensor",
                                 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 room_x, room_y, SDL_WINDOW_SHOWN
                                 );
	renderer = SDL_CreateRenderer(window, -1, 0);

	/* alocando matriz de celulas que representam a sala */
	room = (Cell**) malloc(sizeof(Cell*) * (room_x/res + 1));
    
	/* calculando valores de cada celula */
    for(i = 0; i < (room_x/res + 1) ; i++) {
		/* alocando cada linha da matriz */
		room[i] = (Cell*) malloc(sizeof(Cell) * (room_y/res + 1));
        for(j = 0; j < (room_y/res + 1) ; j++) {
			/* alocando vetor que representa distancia da celula
			para cada sensor */
            room[i][j].sens_dist = (int*) malloc(sizeof(int) * numsens);

			/* atribuindo valores simples */
			room[i][j].r.x = i*res;
			room[i][j].r.y = room_y - (j+1)*res;
			room[i][j].r.w = res;
			room[i][j].r.h = res;
            
			/* calculando distancia da celula para cada sensor
			fazendo a raiz da soma dos quadrados dos lados*/
            for(k = 0; k < numsens; k++)
                room[i][j].sens_dist[k] = (int) pow((pow((room[i][j].r.x - sens[k].x),(float) 2)
										+ pow((room[i][j].r.y - sens[k].y),(float) 2)),(float) 0.5);
		}
	}		
    
    while(1) {
        
		SDL_PollEvent(&e);

		/* sai do loop se programa for abortado */
		if (e.type == SDL_QUIT){
			break;
		}

		/* atualiza valores se eles tiverem sido lidos
		com sucesso */
		if(getval(sens, numsens, batch) == numsens) {
		    SDL_SetRenderDrawColor(renderer, 0xFF,0xFF,0xFF,0x00);
		    SDL_RenderFillRect(renderer, NULL);
		    
		    for(i = 0; i < (room_x/res + 1) ; i++) {
		        for(j = 0; j < (room_y/res + 1) ; j++) {
					tempDen = 0;
					tempNum = 0;
		            color = 0;
					/* faz inverse distance weighting dos valores
					de cada sensor para "descobrir" o valor da
					celula atual */
		            for(k = 0; k < numsens; k++) {
						if(room[i][j].sens_dist[k] == 0) {
							tempNum = sens[k].info;
							tempDen = 1.0;
							break;
						}
		                tempNum += ( sens[k].info	/ pow(room[i][j].sens_dist[k],
														 (float) factor));
						tempDen += ( 1.0			/ pow(room[i][j].sens_dist[k],
														 (float) factor));
		            }
		            
					/* deduz a cor da celula dados o valor e o minimo e o maximo
					possiveis*/
					temp = tempNum/tempDen;
					if(temp < val_min)	temp = 0;
					color = (0xFF) * (temp - val_min)/(val_max - val_min);
					#ifdef _DEBUG
					printf("x:%3d, y:%3d, temp:%.2f, color:%.2f\n", room[i][j].r.x, room[i][j].r.y, temp, color);
					#endif
					if(mode == 't')
						SDL_SetRenderDrawColor(renderer, color,0x00,0x00,0x00);
					else if(mode == 'l')
						SDL_SetRenderDrawColor(renderer, color,color,color,0x00);
					SDL_RenderFillRect(renderer, &(room[i][j].r));
		        }
				SDL_RenderPresent(renderer);
			}
			batch++;
		}
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
	/* liberando espacos alocados */
	free(sens);
	for(i = 0; i < (room_x/res + 1) ; i++) {
		for(j = 0; j < (room_y/res + 1) ; j++)
			free(room[i][j].sens_dist);
		free(room[i]);
	}
	free(room);

	/* DELETAR ARQUIVOS DO DATA */
	system("rm -r Data/*");

    return 0;    
}
