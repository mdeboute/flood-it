
#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <stdbool.h>
#include <stdio.h>

struct Game_grid_s
{
	game  current_game; // current game the user is playing
	uint  cell_size; // cell_size for display
	char *solution; // current_game's solution
	bool  solver; // true if solver mode is activated (showing black dots)
	uint   solution_index; // which move is supposed to be played in the solution
	uint   nb_colors; // number of colors in the current game
};

typedef struct Game_grid_s Game_grid;

struct Env_t
{
	Game_grid *   grid; // contains everything for the grid
	int           grid_x, grid_y;
	int           grid_w, grid_h;

	SDL_Texture * background;
	SDL_Texture **text; // text_zones
	uint           nb_of_text_zones;
	int *         text_x, *text_y;
	int *         rect_x, *rect_y;
	int *         rect_w, *rect_h;
};

typedef struct Env_t Env;

#define new_max(x, y) ((x) >= (y)) ? (x) : (y)
#define new_min(x, y) ((x) <= (y)) ? (x) : (y)

// Render game
void render_menus(int height, int width, SDL_Renderer *ren, Env *env);
void render_grid(Env *env, SDL_Renderer *ren, int w_height, int w_width, bool solve);
void render_text(SDL_Renderer *ren, Env *env, int text_number, int x, int y);

// Render forms
void render_an_empty_square(SDL_Renderer *ren, int tl_x, int tl_y, int br_x, int br_y);
void render_a_full_square(SDL_Renderer *ren, int tl_x, int tl_y, int br_x, int br_y, SDL_Color c);
void render_a_full_circle(SDL_Renderer *renderer, int centreX, int centreY, int radius, SDL_Color c);

// Buttons
bool inside_button(int nb, Env *env, SDL_Point mouse);

// solver
void  sdl_find_min(Env *env);

// main functions
void usage_sdl(char *argv[], char *msg);

#endif