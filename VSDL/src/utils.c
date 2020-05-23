#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_io.h"

char *strrev(char *string)
{
	char *end   = string;
	char *begin = string;
	char  tmp;
	while (*end != '\0')
	{
		end++;
	}
	end--;
	while (end != begin && begin < end)
	{
		tmp    = *end;
		*end   = *begin;
		*begin = tmp;
		begin++;
		end--;
	}
	return string;
}

void print_game(game g, bool ext, int nb_colors) // la fonction fonctionne
{
	printf("nb coups joués : %d ; nb coups max : %d\n", game_nb_moves_cur(g), game_nb_moves_max(g)); // trouver comment recup les coups joués
	for (int y = 0; y < game_height(g); y++)
	{
		for (int x = 0; x < game_width(g); x++)
		{
			printf("%c",
				   (game_cell_current_color(g, x, y) >= 10) ? ('A' + (game_cell_current_color(g, x, y) - 10))
															: (char)(game_cell_current_color(g, x, y) + 48));
		}
		printf("\n");
	}
	if (ext && game_nb_moves_cur(g) == 0)
	{
		printf("Jouer un coup: ");
		char i;
		// print numeros
		for (i = '0'; i < '0' + nb_colors && i <= '9'; i++)
		{
			printf("%c,", i);
		}
		// print lettres
		for (i = 'A'; i < 'A' + nb_colors - 10; i++)
		{
			printf("%c,", i);
		}
		printf("r ou q ; r pour redémarrer ou q pour quitter)\n");
	}
	else if (game_nb_moves_cur(g) == 0 && ext == 0)
	{
		printf("Jouer un coup: (0,1,2,3,r ou q ;  r pour redémarrer ou q pour quitter)\n");
	}
	else
	{
		printf("Jouer un coup: (num couleur%sou r pour redemarrer ou q pour quitter)\n", (nb_colors >= 10) ? ", lettre " : " ");
	}
}

game game_random_ext(uint width, uint height, bool is_wrapping, uint max_color, uint nb_max_moves)
{
	game g = game_new_empty_ext(width, height, is_wrapping);
	for (uint x = 0; x < width; x++)
		for (uint y = 0; y < height; y++)
		{
			uint c = (uint)rand() % max_color;
			game_set_cell_init(g, x, y, c);
		}
	game_restart(g);
	game_set_max_moves(g, nb_max_moves);
	return g;
}

game game_create_grid(uint width, uint height, bool isWrapping, uint nb_colors)
{
	game g = game_new_empty_ext(width, height, isWrapping);
	game_set_max_moves(g, 12);
	for (uint y = 0; y < game_height(g); y++)
	{
		for (uint x = 0; x < game_width(g); x++)
		{
			game_set_cell_init(g, x, y, rand() % nb_colors);
		}
	}
	return g;
}

void usage_recolor(int argc, char *argv[])
{
	fprintf(stderr, "Usage: %s <width> <height> <wrapping: yes/no> <number_colors>\n", argv[0]);
	exit(EXIT_FAILURE);
}

void print_solution_in_file(FILE *file, char *solution)
{
	uint solution_size = strlen(solution);
	for (uint i = 0; i < solution_size; i++)
	{
		fprintf(file, "%c", solution[i]);
		if (i != solution_size - 1)
			fprintf(file, " ");
		else
			fprintf(file, "\n");
	}
	fclose(file);
}

void usage_solver() { printf("recolor_solve FIND_ONE|NB_SOL|FIND_MIN <nom_fichier_pb> <prefix_fichier_sol>\n"); }

char *remove_and_cat(char *filename, char *extension)
{ //Cette fonction verifie si le fichier filename.extension existe, le supprime si oui et renvoit le propre nom du fichier(filename.extension)
	FILE *test_file = fopen(strcat(filename, extension), "r");
	if (test_file != NULL)
	{
		fclose(test_file);
		remove(filename);
	}
	return filename;
}

void append_last_color(uint last_color, char *result)
{
	char toAppend = (last_color >= 0 && last_color <= 9) ? '0' + last_color : 'A' + last_color;
	strncat(result, &toAppend, 1);
}

// ----------------------------- SOLVERS -----------------------------

void find_shortest_depth(const game original_game, uint nb_colors, int *depth)
{
	if (game_is_over(original_game))
	{
		int nb_cur = game_nb_moves_cur(original_game);
		if (nb_cur < *depth) // si on trouve une solution avant le nb de coups max, alors on pose la limite a ce nombre de coups
		{
			*depth = nb_cur;
			return;
		}
	}

	if (game_nb_moves_cur(original_game) == *depth)
	{
		return;
	}

	game game_copied = game_copy(original_game); // On copie la game pour ne pas la modifier

	uint color_before = game_cell_current_color(original_game, 0, 0);
	for (uint i = 0; i < nb_colors; i++) // On parcourt tous les coups jouables (et optimisés)
	{
		if (i != color_before)
		{
			game_play_one_move(game_copied, i);                 // On joue le coup
			find_shortest_depth(game_copied, nb_colors, depth); // On appelle la fonction elle même sur la grille après coup
			game_delete(game_copied);
			game_copied = game_copy(original_game); // On récupère l'ancienne grille pour la prochaine itération
		}
	}
	game_delete(game_copied);
}

bool solver_find_one_aux(const game original_game, uint nb_colors, int depth, char *result)
{
	color color_before = game_cell_current_color(original_game, 0, 0);

	if (game_is_over(original_game))
	{
		if (game_nb_moves_cur(original_game) != 0) // si la game est déjà gagnée, il n'y a pas de solutions
		{
			append_last_color(color_before, result);
			return true;
		}
	}

	if (game_nb_moves_cur(original_game) == depth)
		return false;

	game game_copied = game_copy(original_game); // On copie la game pour ne pas la modifier

	for (uint i = 0; i < nb_colors; i++) // On parcourt tous les coups jouables (et optimisés)
	{
		if (i != color_before)
		{
			game_play_one_move(game_copied, i);                                     // On joue le coup
			bool save = solver_find_one_aux(game_copied, nb_colors, depth, result); // On appelle la fonction elle même sur la grille après coup
			if (save)
			{
				if (game_nb_moves_cur(original_game) != 0)
					append_last_color(color_before, result);
				game_delete(game_copied);
				return true;
			}
			game_delete(game_copied);
			game_copied = game_copy(original_game); // On récupère l'ancienne grille pour la prochaine itération
		}
	}
	game_delete(game_copied);
	return false;
}

char *solver_find_one(const game g, uint nb_colors, bool *hasOne)
{
	uint  nb_max   = game_nb_moves_max(g);
	char *solution = malloc(sizeof(char) * nb_max + 1); // pour '\0'
	solution[0]    = '\0';
	*hasOne        = solver_find_one_aux(g, nb_colors, nb_max, solution);
	strrev(solution);
	return solution;
}

void solver_find_min_aux(const game original_game, uint nb_colors, uint *depth, char *solution, char *temp, uint temp_length, bool *hasOne)
{
	uint  nb_cur       = game_nb_moves_cur(original_game);
	color color_before = game_cell_current_color(original_game, 0, 0);

	if ((!(*hasOne) || nb_cur < *depth) &&
		game_is_over(original_game)) // si j'ai gagné avec une solution plus petite (évite les appels a game vers la fin)
	{
		*hasOne = true;
		*depth  = nb_cur; // depth est aussi égal à la taille de la solution complète
		strcpy(solution, temp);
		return;
	}

	if (nb_cur >= *depth - 1) // si je suis a la taille de ma branche et que juste avant je n'ai pas trouvé de solution
	{                         // dans ce cas je sors de cette branche
		return;
	}

	for (uint i = 0; i < nb_colors; i++)
	{
		if (i != color_before)
		{
			game game_copied = game_copy(original_game);
			game_play_one_move(game_copied, i);
			append_last_color(i, temp);
			solver_find_min_aux(game_copied, nb_colors, depth, solution, temp, (temp_length + 1), hasOne);
			temp[temp_length] = '\0';
			game_delete(game_copied);
		}
	}
}

char *solver_find_min(const game g, uint nb_colors, bool *hasOne)
{
	uint  nb_max   = game_nb_moves_max(g) + 1;
	char *solution = malloc(sizeof(char) * (nb_max + 1)); // +1 pour le \0
	solution[0]    = '\0';
	char temp[nb_max];
	strcpy(temp, "");
	*hasOne = false;
	solver_find_min_aux(g, nb_colors, &nb_max, solution, temp, 0, hasOne);
	return solution;
}

char *solver_find_min_to_get_nb_move_max(const game g, uint nb_colors, bool *hasOne, int times_more, int *solution_size)
{
	uint  nb_max   = game_nb_moves_max(g) + 1 + times_more;
	char *solution = malloc(sizeof(char) * (nb_max + 1)); // +1 pour le \0
	solution[0]    = '\0';
	char *temp     = malloc(sizeof(char) * times_more);
	temp[0]        = '\0';
	*hasOne        = false;
	solver_find_min_aux(g, nb_colors, &nb_max, solution, temp, 0, hasOne);
	*solution_size = strlen(solution);
	free(temp);
	return solution;
}

void solver_nb_solutions_aux(const game original_game, uint nb_colors, int depth, int *nb_solutions) // rec terminale
{
	uint nb_cur = game_nb_moves_cur(original_game);
	if (game_is_over(original_game))
	{
		if (nb_cur != 0) // si la game est déjà gagnée, il n'y a pas de solutions
		{
			(*nb_solutions)++;
			return;
		}
	}

	if (nb_cur == depth)
	{
		return;
	}

	game game_copied = game_copy(original_game); // On copie la game pour ne pas la modifier

	uint color_before = game_cell_current_color(original_game, 0, 0);
	for (uint i = 0; i < nb_colors; i++) // On parcourt tous les coups jouables (et optimisés)
	{
		if (i != color_before)
		{
			game_play_one_move(game_copied, i);                                   // On joue le coup
			solver_nb_solutions_aux(game_copied, nb_colors, depth, nb_solutions); // On appelle la fonction elle même sur la grille après coup
			game_delete(game_copied);
			game_copied = game_copy(original_game); // On récupère l'ancienne grille pour la prochaine itération
		}
	}
	game_delete(game_copied);
}

int solver_nb_solutions(const game original_game, uint nb_colors)
{
	int nb_solutions = 0;
	solver_nb_solutions_aux(original_game, nb_colors, game_nb_moves_max(original_game),
							&nb_solutions); // trouve le nb de solutions de profondeur depth
	return nb_solutions;
}

void check_solution_and_print_accordingly(FILE *file, char *solution, bool hasOne, char **argv)
{
	if (!hasOne)
	{
		printf("No solution found. Saved in \"%s\"\n", argv[3]);
		fprintf(file, "NO SOLUTION\n");
		fclose(file);
	}
	else
	{
		print_solution_in_file(file, solution);
		printf("Done ! Check solution in the file \"%s\"\n", argv[3]);
		printf("Solution is %s\n", solution);
	}
	free(solution);
}

uint get_nb_colors(const game g)
{
	color max = 0, cur_color;
	for (uint x = 0; x < game_width(g); x++)
	{
		for (uint y = 0; y < game_height(g); y++)
		{
			cur_color = game_cell_current_color(g, x, y);
			if (max < cur_color)
				max = cur_color;
		}
	}
	return max + 1;
}
