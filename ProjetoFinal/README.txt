Antes de tudo, instale o SDL2 na maquina de terra.
Para isto, basta rodar o script shell instalacaoSDL.sh

Como configurar o programa:

1) Abrir o arquivo "config.txt"

2) Entrar os valores desejados:

	1o valor: numero de sensores sem fio.
	2o valor: sensor a ser lido: "l" (luz) ou "t" (temperatura)
	
	3o valor: dimensao x da sala.
	4o valor: dimensao y da sala.
	5o valor: resolucao de cada celula.
	6o valor: escala

	7o valor: valor minimo das celulas.
	8o valor: valor maximo das celulas

	9o valor: fator da potencia da inverse distance weighting

3) Repetir essa etapa n vezes, onde n eh o numero de sensores disponiveis:

	1o valor: posicao x do sensor.
	2o valor: posicao y do sensor.
	3o valor: informacao inicial do sensor.
	4o valor: nodeID do sensor.

OBS1:	Os sensores devem ser declarados em config.txt com ordem de nodeID crescente

OBS2:	A unidade das dimensoes da sala sao o milimetro e a da resolucao o pixel, recomenda-se uma escala de 0.1 com resolucao de 10px

OBS3:	A resolucao indica o tamanho de cada celula

OBS4:	O metodo de idw (inverse distance weighting) usado nesse projeto eh o do pastor.

OBS5:	idw eh um metodo de interpolacao que atribui um valor para um ponto fazendo a "media ponderada" de outros pontos, onde o peso de seus valores fica menor conforme a sua distancia do ponto aumenta.
		A potencia acentua a contribuicao de pontos mais proximos e diminue a de pontos mais longinquos

Mais informacoes sobre idw aqui: https://en.wikipedia.org/wiki/Inverse_distance_weighting
Inclusive existe uma imagem nesse artigo que mostra os efeitos de diferentes potencias empegadas para a idw:
https://en.wikipedia.org/wiki/File:Shepard_interpolation_2.png
