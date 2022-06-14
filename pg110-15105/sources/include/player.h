/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef PLAYER_H_
#define PLAYER_H_

#include <SDL/SDL_image.h>
#include <map.h>
#include <constant.h>
#include <game.h>
#include <assert.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>

struct player;

// About the lives of the player
int player_get_nb_lives(struct player *player);
void player_inc_nb_lives(struct player *player);
void player_dec_nb_lives(struct player *player);

// Creates a new player with a given number of available bombs
struct player *player_init(int bomb_number, int lives);
void player_free(struct player *player);

// Set the position of the player
void player_set_position(struct player *player, int x, int y);

// Returns the current position of the player
int player_get_x(struct player *player);
int player_get_y(struct player *player);

// Get and set the time and condition of the player
int player_get_time(struct player *player);
void player_set_time(struct player *player, int time);
int player_get_condition(struct player *player);
void player_set_condition(struct player *player, int cond);

// Set the direction of the next move of the player
void player_set_current_way(struct player *player, enum direction direction);

// Set, Increase, Decrease the number of bomb that player can put
int player_get_nb_bomb(struct player *player);
void player_inc_nb_bomb(struct player *player);
void player_dec_nb_bomb(struct player *player);

// Set, Increase, Decrease the range
void player_dec_rg_bomb(struct player *player);
void player_inc_rg_bomb(struct player *player);
int player_get_rg_bomb(struct player *player);

// Move the player according to the current direction
int player_move(struct player *player, struct map *map);

// Display the player on the screen
void player_display(struct player *player);

// Set, get, increase, decrease the number of keys
int player_get_key(struct player *player);
void player_add_key(struct player *player);
void player_lose_key(struct player *player);
void player_set_key(struct player *player, int num_key);

#endif /* PLAYER_H_ */
