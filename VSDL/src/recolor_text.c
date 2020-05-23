#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"
#include "game_io.h"
#include "utils.h"

//Authors : Martin Debouté, Gonzague Beutter, Paul Brebinaud

int main(int argc, char **argv)
{
	int  nb_colors = NB_COLORS; // by default nb_colors = 4
	game new_game  = NULL;
	if ((argc != 1 && argc != 2 && argc < 4) || argc > 5)
	{
		fprintf(stderr, "Wrong number of arguments\n");
		usage_recolor(argc, argv);
	}
	if (argc == 1) // if there's no argument
	{
		color cells[SIZE * SIZE] = {0, 0, 0, 2, 0, 2, 1, 0, 1, 0, 3, 0, 0, 3, 3, 1, 1, 1, 1, 3, 2, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 0, 3, 3, 2,
									2, 3, 1, 0, 3, 2, 1, 1, 1, 2, 2, 0, 2, 1, 2, 3, 3, 3, 3, 2, 0, 1, 0, 0, 0, 3, 3, 0, 1, 1, 2, 3, 3, 2, 1, 3,
									1, 1, 2, 2, 2, 0, 0, 1, 3, 1, 1, 2, 1, 3, 1, 3, 1, 0, 1, 0, 1, 3, 3, 3, 0, 3, 0, 1, 0, 0, 2, 1, 1, 1, 3, 0,
									1, 3, 1, 0, 0, 0, 3, 2, 3, 1, 0, 0, 1, 3, 3, 1, 1, 2, 2, 3, 2, 0, 0, 2, 2, 0, 2, 3, 0, 1, 1, 1, 2, 3, 0, 1};
		new_game                 = game_new(cells, 12);
		print_game(new_game, false, nb_colors);
	}
	if (argc == 2)
	{
		new_game  = game_load(argv[1]);
		uint maxi = 0;
		for (uint x = 0; x < game_width(new_game); x++)
		{
			for (uint y = 0; y < game_height(new_game); y++)
			{
				if (game_cell_current_color(new_game, x, y) > maxi)
					maxi = game_cell_current_color(new_game, x, y);
			}
		}
		nb_colors = maxi + 1;
		print_game(new_game, game_is_wrapping(new_game), maxi + 1);
	}
	if (argc >= 4) // 3 arguments -> is wrapping
	{
		uint width  = atoi(argv[1]);
		uint height = atoi(argv[2]);
		if (width == 0 || height == 0)
		{
			fprintf(stderr, "Wrong size%sgiven to recolor game\n", (width == 0 && height == 0) ? "s " : " ");
			usage_recolor(argc, argv);
		}
		int testIsWrapping = (strcmp(argv[3], "false") == 0 || strcmp(argv[3], "no") == 0)
			? false
			: (strcmp(argv[3], "true") == 0 || strcmp(argv[3], "yes") == 0) ? true : -1;
		if (testIsWrapping == -1)
		{
			fprintf(stderr, "Can't determine wether wrapping is true or false\n");
			usage_recolor(argc, argv);
		}
		bool isWrapping = testIsWrapping;
		printf("wrapping = %s\n", (isWrapping) ? "true" : "false");
		if (argc == 5) // 4 arguments -> nb_colors
		{
			nb_colors = atoi(argv[4]);
			if (nb_colors <= 1 || nb_colors > 16)
			{
				fprintf(stderr, "Wrong nb_color given to recolor game\n");
				usage_recolor(argc, argv);
			}
		}
		new_game = game_create_grid(width, height, isWrapping, nb_colors);
		print_game(new_game, true, nb_colors);
	}
	while (!game_is_over(new_game))
	{
		int c = getchar();
		if (c == EOF)
		{
			game_delete(new_game);
			return EXIT_SUCCESS;
		}
		if (c == 'q')
		{
			printf("DOMMAGE\n");
			game_delete(new_game);
			return EXIT_SUCCESS;
		}
		if (c == 'r')
		{
			game_restart(new_game);
			print_game(new_game, false, nb_colors);
		}
		if (nb_colors <= 9)
		{
			if (c >= '0' && c <= ('0' + nb_colors - 1))
			{
				c = c - 48;
				game_play_one_move(new_game, c);
				print_game(new_game, false, nb_colors);
			}
		}
		else
		{
			if (c >= '0' && c <= '9')
			{
				c = c - 48;
				game_play_one_move(new_game, c);
				print_game(new_game, false, nb_colors);
			}
			if (c >= 'A' && c <= 'A' + nb_colors - 11)
			{
				c = c - 55;
				game_play_one_move(new_game, c);
				print_game(new_game, nb_colors, false);
			}
		}
	}
	if (game_nb_moves_cur(new_game) <= game_nb_moves_max(new_game))
	{
		printf("BRAVO\n");
	}
	game_delete(new_game);
	return EXIT_SUCCESS;
}
