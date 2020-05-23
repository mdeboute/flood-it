#include "game_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

#define MAXLINELEN 4096

struct game_s
{
	color *cells_init;
	color *cells;
	uint   nb_moves_max;
	uint   nb_moves_cur;
	uint   height;
	uint   width;
	bool   wrapping;
};

game game_load(char *filename)
{
	uint  width, height, nb_max;
	char  wrapping;
	FILE *f = fopen(filename, "r");
	if (f == NULL)
	{
		fprintf(stderr, "file cannot be opened, filename : %s\n", filename);
		exit(EXIT_FAILURE);
	}

	if (fscanf(f, "%u %u %u %c\n", &width, &height, &nb_max, &wrapping) <= 0)
	{
		fprintf(stderr, "File cannot be read, wrong format.\n");
		exit(EXIT_FAILURE);
	}
	color cells[height * width];

	for (int i = 0; i < height * width; i++)
	{
		if (fscanf(f, "%u", &cells[i]) <= 0)
		{
			fprintf(stderr, "File cannot be read, wrong format in cells.\n");
			exit(EXIT_FAILURE);
		}
	}

	game theGame = game_new_ext(width, height, cells, nb_max,
								(wrapping == 'S') ? true : false); // On cree le jeu avec les variables locales declarées au paravant
	fclose(f);
	return theGame;
}

void game_save(cgame g, char *filename)
{
	FILE *savingFile = fopen(filename, "w");
	if (savingFile == NULL)
	{
		fprintf(stderr, "couldn't create a file named \"%s\" in game_save", filename);
		exit(EXIT_FAILURE);
	}

	fprintf(savingFile, "%u %u %u %c\n", game_width(g), game_height(g), game_nb_moves_max(g), (game_is_wrapping(g)) ? 'S' : 'N');

	for (uint x = 0; x < game_height(g); x++)
	{
		for (uint y = 0; y < game_width(g); y++)
		{
			fprintf(savingFile, "%u%s", game_cell_current_color(g, y, x), (y == game_width(g) - 1) ? "\n" : " ");
		}
	}
	fclose(savingFile);
}