#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "game_io.h"
#include "utils.h"

bool test_game_solver()
{
	color cells[12 * 12] = {0, 0, 0, 2, 0, 2, 1, 0, 1, 0, 3, 0, 0, 3, 3, 1, 1, 1, 1, 3, 2, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 0, 3, 3, 2,
							2, 3, 1, 0, 3, 2, 1, 1, 1, 2, 2, 0, 2, 1, 2, 3, 3, 3, 3, 2, 0, 1, 0, 0, 0, 3, 3, 0, 1, 1, 2, 3, 3, 2, 1, 3,
							1, 1, 2, 2, 2, 0, 0, 1, 3, 1, 1, 2, 1, 3, 1, 3, 1, 0, 1, 0, 1, 3, 3, 3, 0, 3, 0, 1, 0, 0, 2, 1, 1, 1, 3, 0,
							1, 3, 1, 0, 0, 0, 3, 2, 3, 1, 0, 0, 1, 3, 3, 1, 1, 2, 2, 3, 2, 0, 0, 2, 2, 0, 2, 3, 0, 1, 1, 1, 2, 3, 0, 1};
	game  new_game       = game_new_ext(12, 12, cells, 12, false);
	uint  nb_colors      = get_nb_colors(new_game);
	bool  hasFound       = false;
	char *solution       = solver_find_one(new_game, nb_colors, &hasFound);

	if (!hasFound)
	{
		fprintf(stderr, "Find one n'a pas trouvé de solution!\n");
		game_delete(new_game);
		free(solution);
		return false;
	}
	int  depth     = strlen(solution);
	game test_game = game_copy(new_game);
	for (uint i = 0; i < depth; i++)
	{
		game_play_one_move(test_game, solution[i] - '0');
	}
	if (!game_is_over(test_game))
	{
		fprintf(stderr, "Find one n'a pas trouvé la bonne solution!\n");
		game_delete(new_game);
		free(solution);
		game_delete(test_game);
		return false;
	}
	game_delete(test_game);
	free(solution);
	solution = solver_find_min(new_game, nb_colors, &hasFound);
	depth    = game_nb_moves_max(new_game);
	if (!hasFound)
	{
		fprintf(stderr, "Find min n'a pas trouvé de solution!\n");
		game_delete(new_game);
		free(solution);
		return false;
	}
	find_shortest_depth(new_game, nb_colors, &depth);
	if (strlen(solution) != depth)
	{
		fprintf(stderr, "Find min n'a pas trouvé la solution de bonne taille!\n");
		game_delete(new_game);
		free(solution);
		return false;
	}
	test_game = game_copy(new_game);
	for (uint i = 0; i < depth; i++)
	{
		game_play_one_move(test_game, solution[i] - '0');
	}
	if (!game_is_over(test_game))
	{
		fprintf(stderr, "Find min n'a pas trouvé la bonne solution!\n");
		game_delete(new_game);
		free(solution);
		game_delete(test_game);
		return false;
	}
	if (solver_nb_solutions(new_game, nb_colors) != 156)
	{
		fprintf(stderr, "Nb sol n'a pas trouvé le bon nombre de solutions!\n");
		game_delete(new_game);
		free(solution);
		game_delete(test_game);
		return false;
	}
	game_delete(new_game);
	free(solution);
	game_delete(test_game);
	return true;
}

void usage(int argc, char *argv[])
{
	fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	if (argc == 1)
		usage(argc, argv);

	// start test
	fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
	bool ok = false;
	if (strcmp("game_solver", argv[1]) == 0)
		ok = test_game_solver();
	else
	{
		fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	// print test result
	if (ok)
	{
		fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
		return EXIT_SUCCESS;
	}
	else
	{
		fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
		return EXIT_FAILURE;
	}
}