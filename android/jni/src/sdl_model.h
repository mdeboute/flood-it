#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include <stdbool.h>
#include "sdl_utils.h"

/* **************************************************************** */

#ifdef __ANDROID__
#define PRINT(STR, ...) do { SDL_Log(STR, ##__VA_ARGS__);  } while(0)
#define ERROR(STR, ...) do { SDL_Log(STR, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)
#else
#define PRINT(STR, ...) do { printf(STR, ##__VA_ARGS__); } while(0)
#define ERROR(STR, ...) do { fprintf(stderr, STR, ##__VA_ARGS__); exit(EXIT_FAILURE); } while(0)
#endif

/* **************************************************************** */

#define APP_NAME "Recolor"
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define DELAY 100

/* **************************************************************** */

// Microsoft Windows default 16-color palette

#define set_gray(arg) do{arg.r = 128; arg.g = 128; arg.b = 128; arg.a = 255;}while(0);  //0
#define set_red(arg) do{arg.r = 255; arg.g = 0; arg.b = 0; arg.a = 255;}while(0);       //1
#define set_lime(arg) do{arg.r = 0; arg.g = 255; arg.b = 0; arg.a = 255;}while(0);      //2
#define set_yellow(arg) do{arg.r = 255; arg.g = 255; arg.b = 0; arg.a = 255;}while(0);  //3
#define set_blue(arg) do{arg.r = 0; arg.g = 0; arg.b = 255; arg.a = 255;}while(0);      //4
#define set_fuchsia(arg) do{arg.r = 255; arg.g = 0; arg.b = 255; arg.a = 255;}while(0); //5
#define set_aqua(arg) do{arg.r = 0; arg.g = 255; arg.b = 255; arg.a = 255;}while(0);    //6
#define set_white(arg) do{arg.r = 255; arg.g = 255; arg.b = 255; arg.a = 255;}while(0); //7

#define set_orange(arg) do{arg.r = 255; arg.g = 165; arg.b =  0; arg.a = 255;}while(0); // 8
#define set_maroon(arg) do{arg.r = 128; arg.g = 0; arg.b = 0; arg.a = 255;}while(0);    //9
#define set_green(arg) do{arg.r = 0; arg.g = 128; arg.b = 0; arg.a = 255;}while(0);     //10
#define set_olive(arg) do{arg.r = 128; arg.g = 128; arg.b = 0; arg.a = 255;}while(0);   //11
#define set_navy(arg) do{arg.r = 0; arg.g = 0; arg.b = 128; arg.a = 255;}while(0);      //12
#define set_purple(arg) do{arg.r = 128; arg.g = 0; arg.b = 128; arg.a = 255;}while(0);  //13
#define set_teal(arg) do{arg.r = 0; arg.g = 128; arg.b = 128; arg.a = 255;}while(0);    //14
#define set_silver(arg) do{arg.r = 192; arg.g = 192; arg.b = 192; arg.a = 255;}while(0);//15

#define set_black(arg) do{arg.r = 0; arg.g = 0; arg.b = 0; arg.a = 255;}while(0);       //solver



#define FONT "Doubt.ttf"
#define FONTSIZE 36
#define BACKGROUND "background.jpg"

#define SDL_GREENC {128,255,0,255}; // vert clair
#define SDL_CYAN {0,255,255, 255}; // cyan
#define SDL_MANGENTA {255, 0, 255, 255}; // mangenta


#define SDL_WHITE {255,255,255,255};
#define SDL_BLACK {0, 0, 0, 255};

/* **************************************************************** */

Env * init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]);
void render(SDL_Window* win, SDL_Renderer* ren, Env * env);
void clean(SDL_Window* win, SDL_Renderer* ren, Env * env);
bool process(SDL_Window* win, SDL_Renderer* ren, Env * env, SDL_Event * e);

/* **************************************************************** */

#endif
