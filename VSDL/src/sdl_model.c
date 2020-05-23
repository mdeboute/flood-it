
#include <SDL.h>
#include <SDL_image.h> // required to load transparent texture from PNG
#include <SDL_ttf.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_io.h"
#include "utils.h"

#include "sdl_model.h"
#include "sdl_utils.h"

/* **************************************************************** */

/* **************************************************************** */

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[])
// Initialize the SDL window and environment
{
	int  nb_of_text_zones = 6;
	int  nb_colors        = 4;     // basic nb_of_colors
	bool is_wrapping      = false; // basic value for wrapping

	Env *env = malloc(sizeof(Env));
	if (!env)
		ERROR("Malloc Error\n");

	env->nb_of_text_zones = nb_of_text_zones;

	env->grid = malloc(sizeof(Game_grid));
	if (!env->grid)
		ERROR("Malloc Error\n");
	//set all pointers to null, indexes to 0 and bools to false initially
	env->grid->current_game   = NULL;
	env->grid->solution       = NULL;
	env->grid->solver         = false;
	env->grid->solution_index = 0;

	if (argc == 0) // if no arguments, classic game
	{
		color cells[SIZE * SIZE] = {0, 0, 0, 2, 0, 2, 1, 0, 1, 0, 3, 0, 0, 3, 3, 1, 1, 1, 1, 3, 2, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 0, 3, 3, 2,
									2, 3, 1, 0, 3, 2, 1, 1, 1, 2, 2, 0, 2, 1, 2, 3, 3, 3, 3, 2, 0, 1, 0, 0, 0, 3, 3, 0, 1, 1, 2, 3, 3, 2, 1, 3,
									1, 1, 2, 2, 2, 0, 0, 1, 3, 1, 1, 2, 1, 3, 1, 3, 1, 0, 1, 0, 1, 3, 3, 3, 0, 3, 0, 1, 0, 0, 2, 1, 1, 1, 3, 0,
									1, 3, 1, 0, 0, 0, 3, 2, 3, 1, 0, 0, 1, 3, 3, 1, 1, 2, 2, 3, 2, 0, 0, 2, 2, 0, 2, 3, 0, 1, 1, 1, 2, 3, 0, 1};
		env->grid->current_game  = game_new(cells, 12);
	}

	else if (argc == 1) // if there is an argument, load the game
	//no need to check for errors, game_load takes care of that
	{
		env->grid->current_game = game_load(argv[1]);
		nb_colors               = get_nb_colors(env->grid->current_game);
	}

	else if (argc >= 3) // if there is at least width, height and nb_colors
	{
		if (argc >= 4)
		{
			nb_colors = atoi(argv[4]);            // get nb color
			if (nb_colors > 16 || nb_colors <= 2) // check if nb_colors has a good value
				usage_sdl(argv, "Number of colors must be 2 or more, and under 16\n");
		}
		if (argc == 5)
		{
			int checker = (strcmp("S", argv[5]) == 0) ? 1 : (strcmp("N", argv[5]) == 0) ? 0 : -1; // check if wrapping arg is S or N
			if (checker == -1)                                                                    // if it's not one of them, exit program
				usage_sdl(argv, "Invalid wrapping argument\n");
			is_wrapping = checker; // if it is, give it to wrapping
		}
		env->grid->current_game =
			game_random_ext(atoi(argv[1]), atoi(argv[2]), is_wrapping, nb_colors, atoi(argv[3])); // create the random game with the arguments
	}

	env->grid->nb_colors = nb_colors; // save nb_colors for future use

	PRINT("Welcome to recolor.\n");
	PRINT("Press ESC to exit program.\n");

	/* get current window size */
	int w, h;
	SDL_GetWindowSize(win, &w, &h);

	// loads background

	env->background = IMG_LoadTexture(ren, BACKGROUND);
	if (!env->background)
		ERROR("Cannot find image %s", BACKGROUND);

	// loads text zones
	env->text = calloc(nb_of_text_zones, sizeof(SDL_Texture *));
	if (!env->text)
		ERROR("Malloc Error\n");
	// allocates the text and rectangle positions while setting them to 0
	env->text_x = calloc(nb_of_text_zones, sizeof(int));
	env->text_y = calloc(nb_of_text_zones, sizeof(int));
	env->rect_x = calloc(nb_of_text_zones, sizeof(int));
	env->rect_y = calloc(nb_of_text_zones, sizeof(int));
	env->rect_w = calloc(nb_of_text_zones, sizeof(int));
	env->rect_h = calloc(nb_of_text_zones, sizeof(int));

	return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env)
//Renders the visual stuff
{
	/* get current window size */
	int w, h;
	SDL_GetWindowSize(win, &w, &h);

	int bottom_menu_height = h - (h / 23);
	int ch                 = (h - (h / 6) - 2) / game_height(env->grid->current_game); // max possible cell height
	int cw                 = w / game_width(env->grid->current_game);                  // max possible cell width
	env->grid->cell_size   = new_min(ch, cw);                                          // get the minimum to fit the screen, and to have squares

	env->grid_h = game_height(env->grid->current_game) * env->grid->cell_size;
	env->grid_w = game_width(env->grid->current_game) * env->grid->cell_size;

	// background
	SDL_RenderCopy(ren, env->background, NULL, NULL); /* stretch it */

	// render game

	render_menus(h, w, ren, env);

	render_grid(env, ren, h, w, env->grid->solver);

	if (game_is_over(env->grid->current_game))
	{
		render_text(ren, env, 5, w / 2, h / 2); // render "you won" text
	}

	// render all texts
	render_text(ren, env, 4, w / 6, h / 20);                 // NEW GAME
	render_text(ren, env, 0, w / 2, h / 20);                 // Number of moves done / nb of moves max
	render_text(ren, env, 1, w / 4, bottom_menu_height);     // SOLVER
	render_text(ren, env, 2, w / 2, bottom_menu_height);     // RESTART
	render_text(ren, env, 3, 3 * w / 4, bottom_menu_height); // QUIT
}

/* **************************************************************** */

bool check_all_buttons(Env *env, SDL_Point mouse, uint w, uint h)
{
	uint height = game_height(env->grid->current_game);
	uint width  = game_width(env->grid->current_game);

	if (inside_button(4, env, mouse)) // if inside the new game button
	{
		uint nb_max = game_nb_moves_max(env->grid->current_game);

		game_delete(env->grid->current_game);
		env->grid->current_game = game_random_ext(width, height, false, env->grid->nb_colors, nb_max);

		if (env->grid->solution != NULL) // if there exists already a solution, destroy it
		{
			free(env->grid->solution);
			env->grid->solution = NULL;
			env->grid->solver   = false;
		}
	}

	if (inside_button(1, env, mouse)) // if inside the solver button
	{
		if (env->grid->solution == NULL) // if there is no solution
		{
			PRINT("Solver initialized. This may take a while.\n");
			bool hasOne         = false;
			env->grid->solution = solver_find_min(env->grid->current_game, get_nb_colors(env->grid->current_game), &hasOne);
			// find_min to get a decent solution

			if (!hasOne) // if it did not find any solution
			{
				PRINT("NO Solution! Create a new grid.\n");
				free(env->grid->solution);  // free the pointer which was allocated
				env->grid->solution = NULL; // set to NULL to prevent free errors
			}
			else
				PRINT("Solution found, check the grid. sol: %s\n", env->grid->solution);
		}
		if (env->grid->solution != NULL) // If there is already a solution, show it
		{
			env->grid->solver = true;
		}
	}
	if (inside_button(2, env, mouse)) // if inside the restart button
	{
		env->grid->solution_index = 0;     // restarts actual solution index
		env->grid->solver         = false; // stops the display of the solution
		if ((env->grid->solution != NULL) &&
			(strlen(env->grid->solution) !=
			 game_nb_moves_max(env->grid->current_game))) // if the solution is for a specified case (shorter than nb_move_max)
		{
			free(env->grid->solution);  // free the pointer which was allocated
			env->grid->solution = NULL; // set to NULL to prevent free errors
		}
		game_restart(env->grid->current_game);
	}
	if (inside_button(3, env, mouse)) // if inside the quit button
	{
		return true;
	}

	// if a cell is clicked
	if (!game_is_over(env->grid->current_game)) // shouldn't play anymore if game is over but the other buttons are available
	{
		if (mouse.x >= env->grid_x && mouse.x <= env->grid_x + game_width(env->grid->current_game) * env->grid->cell_size && mouse.y >= env->grid_y &&
			mouse.y <= env->grid_y + game_height(env->grid->current_game) * env->grid->cell_size)
		{                                                                           // if user clicked in the game grid
			uint cell_x = (mouse.x - (w - env->grid_w) / 2) / env->grid->cell_size; // get x position of the clicked cell
			uint cell_y = (mouse.y - (h - env->grid_h) / 2) / env->grid->cell_size; // get its y position

			color played_color = game_cell_current_color(env->grid->current_game, cell_x, cell_y); // get the color that the user wants to play

			if (played_color == game_cell_current_color(env->grid->current_game, 0, 0)) // if the user plays the same color, abort
				return false;

			if (env->grid->solver &&
				(played_color !=
				 (env->grid->solution[env->grid->solution_index] - '0'))) // if the user clicks another color than the right one, abort solver
			{
				env->grid->solver         = false;
				env->grid->solution_index = 0;
				free(env->grid->solution);
				env->grid->solution = NULL;
			}

			game_play_one_move(env->grid->current_game, played_color); // We finally play the color

			if (env->grid->solver) // if is in solving mode and played the right move, increment index
				env->grid->solution_index++;

			if (game_is_over(env->grid->current_game))
				env->grid->solver = false; // if game is over user doesn't need the help of the solver anymore
		}
	}
	return false;
}

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e)
// Performs all the checks and verifications
{
	int w, h;
	SDL_GetWindowSize(win, &w, &h);

	if (e->type == SDL_QUIT)
	{
		return true;
	}
#ifdef __ANDROID__
	else if (e->type == SDL_FINGERDOWN)
	{
		SDL_Point mouse;
		mouse.x = e->tfinger.x * w;
		mouse.y = e->tfinger.y * h;
		return check_all_buttons(env, mouse, w, h);
	}
	/* other events */
#else
	else if (e->type == SDL_MOUSEBUTTONDOWN) // if the mouse button is pressed
	{
		SDL_Point mouse;
		SDL_GetMouseState(&mouse.x, &mouse.y);
		return check_all_buttons(env, mouse, w, h);
	}
#endif
	else if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.sym)
		{
			case SDLK_ESCAPE: // pressing escape quits the game and closes the window
				return true;
				break;
			case SDLK_s: // pressing key 's' stops showing the solver
				env->grid->solver = false;
				break;
		}
	}
	return false;
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env)
// Cleans everything
{
	// free grid
	SDL_DestroyTexture(env->background);
	if (env->grid->solution)
		free(env->grid->solution);

	// destroy game
	game_delete(env->grid->current_game);

	// destroy text zones
	for (uint i = 0; i < env->nb_of_text_zones; i++)
	{
		if (env->text[i])
			SDL_DestroyTexture(env->text[i]);
	}
	free(env->text);

	// destroy grid
	free(env->grid);

	// free arrays used for text zones
	free(env->text_x);
	free(env->text_y);
	free(env->rect_x);
	free(env->rect_y);
	free(env->rect_w);
	free(env->rect_h);

	free(env);
}

/* **************************************************************** */
