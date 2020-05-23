#version minimale de cmake nécessaire
cmake_minimum_required(VERSION 3.0)
#nom du projet
project ("recolor_text" LANGUAGES C)
#définit le nom du programme ainsi que
#ses sources
include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "-std=c99 -Wall") ### options communes
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG --coverage -fsanitize=address") ### options spécifiques -fsanitize=address pour memleaks
set(CMAKE_C_FLAGS_RELEASE "-O3") ### options spécifiques

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMG REQUIRED SDL2_image)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
set(SDL2_ALL_LIBS ${SDL2_LIBRARIES} ${SDL2_IMG_LIBRARIES} ${SDL2_TTF_LIBRARIES})
set(SDL2_ALL_INC ${SDL2_INCLUDE_DIRS} ${SDL2_IMG_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})


include_directories(${SDL2_ALL_INC})
include_directories(include)

set(SOURCES src/)
set(ASSETS assets/)

add_library(game ${SOURCES}game.c ${SOURCES}game_io.c)
add_library(utils ${SOURCES}utils.c)
add_library(rec_sdl ${SOURCES}sdl_model.c ${SOURCES}sdl_utils.c)
#COPY ASSETS IN BUILD
file(COPY ${ASSETS}
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#EXECUTABLE MAIN
add_executable(recolor_sdl ${SOURCES}recolor_sdl.c)
target_link_libraries(recolor_sdl ${SDL2_ALL_LIBS} m rec_sdl game utils)

add_executable(recolor_text ${SOURCES}recolor_text.c)
target_link_libraries(recolor_text m game utils)

add_executable(recolor_solve ${SOURCES}recolor_solver.c)
target_link_libraries(recolor_solve m game utils)

#EXECUTABLES TEST

add_executable(test_game_V2 ${SOURCES}test_game_V2.c)
add_executable(test_game_io ${SOURCES}test_game_io.c)
add_executable(test_game_solver ${SOURCES}test_game_solver.c)
add_executable(game_io_tester ${SOURCES}game_io_tester.c)

target_link_libraries(test_game_V2 m game utils)
target_link_libraries(test_game_io m game utils)
target_link_libraries(test_game_solver m game utils)
target_link_libraries(game_io_tester m game utils)


add_test(test_game_V2.game_new_ext ./test_game_V2 game_new_ext)
add_test(test_game_V2.game_new_empty_ext ./test_game_V2 game_new_empty_ext)
add_test(test_game_V2.game_is_wrapping ./test_game_V2 game_is_wrapping)
add_test(test_game_V2.game_height ./test_game_V2 game_height)
add_test(test_game_V2.game_width ./test_game_V2 game_width)
add_test(test_game_V2.game_set_max_moves  ./test_game_V2 game_set_max_moves)
add_test(test_game_V2.game_nb_moves_max  ./test_game_V2 game_nb_moves_max)
add_test(test_game_V2.game_cell_current_color  ./test_game_V2 game_cell_current_color)
add_test(test_game_V2.game_nb_moves_cur  ./test_game_V2 game_nb_moves_cur)
add_test(test_game_V2.game_play_one_move  ./test_game_V2 game_play_one_move)
add_test(test_game_V2.game_copy  ./test_game_V2 game_copy)
add_test(test_game_V2.game_set_cell_init  ./test_game_V2 game_set_cell_init)
add_test(test_game_V2.game_is_over  ./test_game_V2 game_is_over)
add_test(test_game_V2.game_restart  ./test_game_V2 game_restart)
add_test(test_game_V2.game_delete  ./test_game_V2 game_delete)
add_test(test_game_solver  ./test_game_solver game_solver)

add_test(test_game_io ./test_game_io game_io)
