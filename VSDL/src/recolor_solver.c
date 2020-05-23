#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"
#include "game_io.h"
#include "utils.h"

#define TEST 0
#if TEST == 1

int max(int a, int b)
{
	return (a > b) ? a : b;
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	uint nb_colors = 8;
	int height = 6 + rand()%10;
	int width = 6 + rand()%10;

	printf("nb_col : %d, height : %d, width : %d\n", nb_colors, height, width);

	int mov_max = (max(height, width) * (nb_colors/4)) + nb_colors/2;
	printf("mov max : %d\n", mov_max);
	game new_game = game_random_ext(width,height,false, nb_colors, mov_max);
	bool hasOne = false;
	solver_find_one(new_game, 4, &hasOne);
	printf("%s\n", (hasOne) ? "oui" : "non");
	game_delete(new_game);

	return EXIT_SUCCESS;
}

#else

int main(int argc, char **argv)
{
	// nom_fichier_pb -> nom du fichier à charger
	// prefix_fichier_sol -> nom du fichier dans lequel sauvegarder
	srand(time(NULL));
	if (argc == 4)
	{
		char *solution;
		game  loaded_game;
		uint  nb_sol;
		argc--;
		loaded_game    = game_load(argv[2]);
		uint nb_colors = get_nb_colors(loaded_game);

		if (strcmp(argv[1], "FIND_ONE") == 0)
		{
			printf("Starting solver\n");
			FILE *saving_file = fopen(remove_and_cat(argv[3], ".sol"), "w");
			bool  hasFound    = false;
			char *solution    = solver_find_one(loaded_game, nb_colors, &hasFound);
			check_solution_and_print_accordingly(saving_file, solution, hasFound, argv);
		}

		else if (strcmp(argv[1], "NB_SOL") == 0)
		{
			FILE *sol = fopen(remove_and_cat(argv[3], ".nbsol"), "w");
			printf("Starting solver\n");
			nb_sol = solver_nb_solutions(loaded_game, nb_colors);
			fprintf(sol, "NB_SOL = %d\n", nb_sol);
			fclose(sol);
			printf("Done ! The number of solutions is in the file \"%s\"\n", argv[3]);
			printf("Number of solutions is %d\n", nb_sol);
		}

		else if (strcmp(argv[1], "FIND_MIN") == 0)
		{
			FILE *saving_file = fopen(remove_and_cat(argv[3], ".sol"), "w");
			bool  hasFound    = false;
			solution          = solver_find_min(loaded_game, nb_colors, &hasFound);
			check_solution_and_print_accordingly(saving_file, solution, hasFound, argv);
		}
		else
		{
			printf("Please enter a proper mode name !\n");
			usage_solver();
		}
		game_delete(loaded_game);
	}
	else
	{
		usage_solver();
	}

	return EXIT_SUCCESS;
}

#endif
