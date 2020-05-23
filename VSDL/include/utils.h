#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

char *strrev(char *string);
void print_game(game g, bool ext, int nb_colors);
game game_random_ext(uint width, uint height, bool is_wrapping, uint max_color, uint nb_max_moves);
game game_create_grid(uint width, uint height, bool isWrapping, uint nb_colors);
void print_solution_in_file(FILE *file, char *solution);
// usage
void usage_recolor(int argc, char *argv[]);
void usage_solver();
char *remove_and_cat(char *filename, char *extension);
void check_solution_and_print_accordingly(FILE * file, char * solution, bool hasOne, char ** argv);
uint get_nb_colors(game g);
void append_last_color(uint last_color, char *result);

// SOLVER FUNCTIONS
char * solver_find_one(const game g, uint nb_colors, bool * hasOne);
int solver_nb_solutions(const game original_game, uint nb_colors);
char *solver_find_min(const game g, uint nb_colors, bool *hasOne);

// SOLVER FOR SDL
char *solver_find_min_to_get_nb_move_max(const game g, uint nb_colors, bool *hasOne, int times_more, int *solution_size);

// TESTS
void find_shortest_depth(const game original_game, uint nb_colors, int *depth);

#endif // __UTILS_H__