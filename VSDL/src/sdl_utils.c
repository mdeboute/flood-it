#include <SDL.h>
#include <SDL_image.h> // required to load transparent texture from PNG
#include <SDL_ttf.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_io.h"
#include "utils.h"

#include "sdl_model.h"
#include "sdl_utils.h"

void add_text(SDL_Renderer *ren, TTF_Font *font, SDL_Color c, Env *env, char *text, int text_number)
// adds the text into the environment, on the spot specified(text_number)
{
	SDL_Surface *surf      = TTF_RenderText_Blended(font, text, c); // blended rendering for ultra nice text
	env->text[text_number] = SDL_CreateTextureFromSurface(ren, surf);
	SDL_FreeSurface(surf);
}

bool inside_button(int nb, Env *env, SDL_Point mouse)
// checks if the mouse click is inside the button of the specified number
{
	return (mouse.x > env->rect_x[nb] && mouse.x < env->rect_x[nb] + env->rect_w[nb] && mouse.y > env->rect_y[nb] &&
			mouse.y < env->rect_y[nb] + env->rect_h[nb]);
}

void render_text(SDL_Renderer *ren, Env *env, int text_number, int x, int y)
// print a text contained in the environment
{
	SDL_Rect rect;
	SDL_QueryTexture(env->text[text_number], NULL, NULL, &rect.w, &rect.h);
	env->text_x[text_number] = x;
	env->text_y[text_number] = y;
	rect.x                   = env->text_x[text_number] - (rect.w / 2); // in the middle of the coordinates given
	rect.y                   = env->text_y[text_number] - (rect.h / 2);
	env->rect_h[text_number] = rect.h;
	env->rect_w[text_number] = rect.w;
	env->rect_x[text_number] = rect.x;
	env->rect_y[text_number] = rect.y;
	SDL_RenderCopy(ren, env->text[text_number], NULL, &rect);
}

// Render game

void render_menus(int height, int width, SDL_Renderer *ren, Env *env)
// Renders the menus (practically adds the texts and renders the visual stuff)
{
	// solution plus courte que je viens de trouver
	char text[256];

	int nb_cur = game_nb_moves_cur(env->grid->current_game);
	int nb_max = game_nb_moves_max(env->grid->current_game);
	sprintf(text, "Moves : %d / %d", nb_cur, nb_max);

	SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE}; /* black color in RGBA */
	TTF_Font *font  = TTF_OpenFont(FONT, (new_min(width, height)) / 18);
	//add of texts
	add_text(ren, font, color, env, text, 0);                   // nb moves cur/max
	add_text(ren, font, color, env, "Solver", 1);               // Solver text
	add_text(ren, font, color, env, "Restart", 2);              // Restart text
	add_text(ren, font, color, env, "Quit", 3);                 // Quit button
	set_black(color);                                           // sets color to black
	add_text(ren, font, color, env, "Good Job ! You won !", 5); // Winning game text
	color.b = 255;
	color.g = 255;                                  //sets color to aqua
	add_text(ren, font, color, env, "New game", 4); //New game text

	TTF_CloseFont(font);
	int th = height - height / 12; //Top height of 2nd menu
	for (int i = 1; i < 4; i++)
	// Renders all visual squares for the menus , aswell as vertical lines
	{
		render_an_empty_square(ren, i, i, width - 1 - i, height / 12 + i - 1);     //first menu
		render_an_empty_square(ren, 1, th + i - 1, width - 1 - i, height - i - 1); //second menu
		SDL_RenderDrawLine(ren, width - 1 - i, i, width - 1 - i, height - i - 1);  // Trait Vertical droit
		SDL_RenderDrawLine(ren, i, i, i, height - i - 1);                          // Trait Vertical Gauche
	}
}

void render_grid(Env *env, SDL_Renderer *ren, int w_height, int w_width, bool solve)
// renders the game grid
{
	int height = game_height(env->grid->current_game);
	int width  = game_width(env->grid->current_game);

	int og_value = w_width / 2 - width / 2 * env->grid->cell_size; // to restart x

	if (width % 2 == 1)
		og_value -= env->grid->cell_size / 2;
	env->grid_x = og_value; // set grid_x on the starting x

	int top_x = og_value;
	int top_y = w_height / 2 - height / 2 * env->grid->cell_size;

	if (height % 2 == 1)
		top_y -= env->grid->cell_size / 2;
	env->grid_y = top_y; // set grid_y on the top_y of the grid

	int       cell_size = env->grid->cell_size;
	SDL_Color color     = {0, 0, 0, 0};

	for (int y = 0; y < height; y++) // render each cell using the right color
	{
		for (int x = 0; x < width; x++)
		{
			switch (game_cell_current_color(env->grid->current_game, x, y))
			{
				case 0:
					set_gray(color);
					break;
				case 1:
					set_red(color);
					break;
				case 2:
					set_lime(color);
					break;
				case 3:
					set_yellow(color);
					break;
				case 4:
					set_blue(color);
					break;
				case 5:
					set_fuchsia(color);
					break;
				case 6:
					set_aqua(color);
					break;
				case 7:
					set_white(color);
					break;
				case 8:
					set_orange(color);
					break;
				case 9:
					set_maroon(color);
					break;
				case 10:
					set_green(color);
					break;
				case 11:
					set_olive(color);
					break;
				case 12:
					set_navy(color);
					break;
				case 13:
					set_purple(color);
					break;
				case 14:
					set_teal(color);
					break;
				case 15:
					set_silver(color);
					break;
				default:
					ERROR("Invalid color in game\n");
					break;
			}
			render_a_full_square(ren, top_x, top_y, top_x + cell_size, top_y + cell_size, color);
			top_x += cell_size;
		}
		top_x = og_value;
		top_y += cell_size;
	}
	if (solve)
	// if the user has enabled the solver
	{
		SDL_Color c;
		set_black(c);
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (game_cell_current_color(env->grid->current_game, x, y) == (env->grid->solution[env->grid->solution_index] - '0'))
				{
					// render black dots to distinguish the solution cells
					render_a_full_circle(ren, (env->grid_x + (env->grid->cell_size * (x + 1)) - env->grid->cell_size / 2),
										 (env->grid_y + (env->grid->cell_size * (y + 1)) - env->grid->cell_size / 2), env->grid->cell_size / 10, c);
				}
			}
		}
	}
}

// Render forms

void render_an_empty_square(SDL_Renderer *ren, int tl_x, int tl_y, int br_x, int br_y)
// renders an empty square(not filled)
{
	SDL_RenderDrawLine(ren, tl_x, tl_y, tl_x, br_y); // Trait horizontal haut
	SDL_RenderDrawLine(ren, tl_x, br_y, br_x, br_y); // Trait Vertical droit
	SDL_RenderDrawLine(ren, br_x, br_y, br_x, tl_y); // Trait Vertical Gauche
	SDL_RenderDrawLine(ren, tl_x, tl_y, br_x, tl_y); // Trait horizontal bas
}

void render_a_full_square(SDL_Renderer *ren, int tl_x, int tl_y, int br_x, int br_y, SDL_Color c)
// renders a full square(filled with the color given in parameter)
{
	SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
	for (int y = 0; y < br_y - tl_y; y++)
	{
		SDL_RenderDrawLine(ren, tl_x, tl_y + y, br_x, tl_y + y); // Trait horizontal
	}
}

void render_a_full_circle(SDL_Renderer *ren, int centreX, int centreY, int radius, SDL_Color c)
// renders a full circle(filled with the color given in parameter)
{
	const int diameter = (radius * 2);
	SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);

	int x     = (radius - 1);
	int y     = 0;
	int tx    = 1;
	int ty    = 1;
	int error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(ren, centreX + x, centreY - y);
		SDL_RenderDrawPoint(ren, centreX + x, centreY + y);
		SDL_RenderDrawPoint(ren, centreX - x, centreY - y);
		SDL_RenderDrawPoint(ren, centreX - x, centreY + y);
		SDL_RenderDrawPoint(ren, centreX + y, centreY - x);
		SDL_RenderDrawPoint(ren, centreX + y, centreY + x);
		SDL_RenderDrawPoint(ren, centreX - y, centreY - x);
		SDL_RenderDrawPoint(ren, centreX - y, centreY + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
	for (double dy = 1; dy <= radius; dy += 1.0)
	{
		double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy))); 
		SDL_RenderDrawLine(ren, centreX - dx, centreY + dy - radius, centreX + dx, centreY + dy - radius);
		SDL_RenderDrawLine(ren, centreX - dx, centreY - dy + radius, centreX + dx, centreY - dy + radius);
	}
}

// tried to optimize, didn't work
// not used in our file

void sdl_find_min_aux(Env *env, game copy_one, game copy_two, char *temp, int temp_length, char *solution, bool *hasOne, int *depth)
{
	uint  nb_cur           = game_nb_moves_cur(copy_one);
	color color_before_cpy = game_cell_current_color(copy_two, 0, 0);

	if ((!(*hasOne) || nb_cur < *depth) &&
		game_is_over(copy_one)) // si j'ai gagné avec une solution plus petite (évite les appels a game vers la fin)
	{
		*hasOne = true;
		*depth  = nb_cur; // depth est aussi égal à la taille de la solution complète
		strcpy(solution, temp);
		return;
	}

	if (nb_cur == *depth - 1) // si je suis a la taille de ma branche et que juste avant je n'ai pas trouvé de solution
	{                         // dans ce cas je sors de cette branche
		return;
	}

	for (uint x = 0; x < game_width(copy_one); x++)
	{
		for (uint y = 0; y < game_height(copy_one); y++)
		{
			game_set_cell_init(copy_two, x, y, game_cell_current_color(copy_one, x, y));
		}
	}
	game_restart(copy_two);
	//print_game(copy_one, true, env->grid->nb_colors);
	for (uint i = 0; i < game_nb_moves_cur(copy_one); i++)
	{
		game_play_one_move(copy_two, color_before_cpy); // ca va augmenter nb cur
	}

	for (uint i = 0; i < env->grid->nb_colors; i++)
	{
		if (i != color_before_cpy)
		{
			append_last_color(i, temp);
			game_play_one_move(copy_two, i);
			sdl_find_min_aux(env, copy_one, copy_two, temp, temp_length + 1, solution, hasOne, depth);
			game_restart(copy_two);
			for (uint i = 0; i < game_nb_moves_cur(copy_one); i++)
			{
				game_play_one_move(copy_two, color_before_cpy); // ca va augmenter nb cur
			}
			temp[temp_length] = '\0';
		}
	}
}

void sdl_find_min(Env *env)
{
	int nb_max             = game_nb_moves_max(env->grid->current_game);
	env->grid->solution    = malloc(sizeof(char) * nb_max + 1);
	env->grid->solution[0] = '\0';
	char temp[nb_max + 1];
	temp[0]       = '\0';
	bool hasOne   = false;
	game copy_one = game_copy(env->grid->current_game);
	game copy_two = game_copy(copy_one);
	sdl_find_min_aux(env, copy_one, copy_two, temp, 0, env->grid->solution, &hasOne, &nb_max);
	printf("%s\n", env->grid->solution);
}

void usage_sdl(char *argv[], char *msg)
// prints the right parameters if the user has made a mistake
{
	fprintf(stderr, "%s <game_file/width> <height> <nb_max_moves> (optional ->) <nb_max_color> <S/N>\n", argv[0]);
	if (!msg)
		exit(EXIT_FAILURE);
	else
		ERROR("%s", msg);
}
