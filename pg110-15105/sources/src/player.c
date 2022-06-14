/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include <unistd.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <map.h>
#include <game.h>
#include <bomb.h>

struct player
{
	int x, y;
	enum direction direction;
	int bombs;
	int rg_bomb;
	int lives;
	int key;
	char level;
	int condition; // 1 = invincible, 0 = normal
	int time;
};

struct player *player_init(int bomb_number, int lives)
{
	struct player *player = malloc(sizeof(*player));
	if (!player)
	{
		error("Memory error");
	}
	player->direction = SOUTH;
	player->bombs = bomb_number;
	player->rg_bomb = 1;
	player->lives = lives;
	player->key = 4; // provisoire
	player->level = '1';
	return player;
}

// debut clé
int player_get_key(struct player *player)
{
	return player->key;
}

void player_add_key(struct player *player)
{
	player->key++;
}

void player_lose_key(struct player *player)
{
	if (player->key >= 1)
	{
		player->key--;
	}
}

void player_set_key(struct player *player, int num_key)
{
	player->key = num_key;
}
// fin clé

void player_set_position(struct player *player, int x, int y)
{
	assert(player);
	player->x = x;
	player->y = y;
}

void player_free(struct player *player)
{
	assert(player);
	free(player);
}

int player_get_x(struct player *player)
{
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player *player)
{
	assert(player != NULL);
	return player->y;
}

void player_set_current_way(struct player *player, enum direction way)
{
	assert(player);
	player->direction = way;
}

int player_get_nb_bomb(struct player *player)
{
	assert(player);
	return player->bombs;
}

void player_inc_nb_bomb(struct player *player)
{
	assert(player);
	player->bombs += 1;
}

void player_dec_nb_bomb(struct player *player)
{
	assert(player);
	if (player->bombs >= 2)
	{
		player->bombs -= 1;
	}
}

void player_inc_rg_bomb(struct player *player)
{
	assert(player);
	player->rg_bomb += 1;
}

void player_dec_rg_bomb(struct player *player)
{
	assert(player);
	player->rg_bomb -= 1;
}

int player_get_rg_bomb(struct player *player)
{
	assert(player);
	return player->rg_bomb;
}

int player_get_time(struct player *player)
{
	assert(player);
	return player->time;
}

void player_set_time(struct player *player, int time)
{
	assert(player);
	player->time = time;
}

int player_get_condition(struct player *player)
{
	assert(player);
	return player->condition;
}

void player_set_condition(struct player *player, int cond)
{
	assert(player);
	player->condition = cond;
}

int player_get_nb_lives(struct player *player)
{
	assert(player);
	return (player->lives);
}

void player_inc_nb_lives(struct player *player)
{
	assert(player);
	player->lives += 1;
}

void player_dec_nb_lives(struct player *player)
{
	assert(player);
	player->lives -= 1;
}

static int player_move_aux(struct player *player, struct map *map, int x, int y)
{

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y))
	{
	case CELL_SCENERY: // prevent the player from walking on a rock or a tree
		return 0;
		break;

	case CELL_BOX: // must allow the player to push a single box + keeping the box inside the boudaries of the map
		switch (player->direction)
		{
		case NORTH:
			if ((map_is_inside(map, x, y - 1)) && (map_get_cell_type(map, x, y - 1) == CELL_EMPTY))
			{ // the order of the test is important so the game won't crash
				map_set_cell_type(map, x, y, CELL_EMPTY);
				map_set_cell_type(map, x, y - 1, CELL_BOX);
				return 1;
			}
			break;
		case SOUTH:
			if ((map_is_inside(map, x, y + 1)) && (map_get_cell_type(map, x, y + 1) == CELL_EMPTY))
			{
				map_set_cell_type(map, x, y, CELL_EMPTY);
				map_set_cell_type(map, x, y + 1, CELL_BOX);
				return 1;
			}
			break;
		case EAST:
			if ((map_is_inside(map, x + 1, y)) && (map_get_cell_type(map, x + 1, y) == CELL_EMPTY))
			{
				map_set_cell_type(map, x, y, CELL_EMPTY);
				map_set_cell_type(map, x + 1, y, CELL_BOX);
				return 1;
			}
			break;
		case WEST:
			if ((map_is_inside(map, x - 1, y)) && (map_get_cell_type(map, x - 1, y) == CELL_EMPTY))
			{
				map_set_cell_type(map, x, y, CELL_EMPTY);
				map_set_cell_type(map, x - 1, y, CELL_BOX);
				return 1;
			}
			break;
		}

		return 0;
		break;

	case CELL_BONUS:
		if (map_get_compose_type(map, x, y) == CELL_BONUS_BOMBINC)
		{
			if (player_get_nb_bomb(player) >= 2)
			{
				player_dec_nb_bomb(player);
				map_set_cell_type(map, x, y, CELL_EMPTY);
			}
			else
			{
				map_set_cell_type(map, x, y, CELL_EMPTY);
			}
		}

		if (map_get_compose_type(map, x, y) == CELL_BONUS_RANGEDEC)
		{
			if (player_get_rg_bomb(player) > 1)
			{
				player_dec_rg_bomb(player);
				map_set_cell_type(map, x, y, CELL_EMPTY);
			}
			else
			{
				map_set_cell_type(map, x, y, CELL_EMPTY);
			}
		}

		if (map_get_compose_type(map, x, y) == CELL_BONUS_BOMBDEC)
		{
			player_inc_nb_bomb(player);
			map_set_cell_type(map, x, y, CELL_EMPTY);
		}
		if (map_get_compose_type(map, x, y) == CELL_BONUS_RANGEINC)
		{
			player_inc_rg_bomb(player);
			map_set_cell_type(map, x, y, CELL_EMPTY);
		}
		if (map_get_compose_type(map, x, y) == CELL_BONUS_LIFE)
		{
			player_inc_nb_lives(player);
			map_set_cell_type(map, x, y, CELL_EMPTY);
		}
		break;

	case CELL_MONSTER:

		if (player->condition == 1)
		{
			return 1;
		}
		else
		{
			if (player->lives >= 1)
			{
				printf("in monster\n");
				player_dec_nb_lives(player);
				player->condition = 1;
				player->time = SDL_GetTicks();
				return 1;
			}
			else
			{
				printf("dead\n");
				// faire le game over
				// game_break();
				// window_display_image(sprite_game_over(), 5, 6);
			}
		}

		break;

	case CELL_DOOR:
		if (map_get_door_type(map, x, y) == DOOR_CLOSED)
		{
			if (player->key >= 1)
			{
				map_set_door_type(map, x, y, DOOR_OPENED);
				player_lose_key(player);
			}
		}
		break;

	case CELL_KEY:
		player->key++;
		map_set_cell_type(map, x, y, CELL_EMPTY);
		break;

	case CELL_BOMB:
		switch (map_get_compose_type(map, x, y))
		{
		case CELL_EXPLOSION:
			if (player_get_nb_lives(player) >= 1)
			{
				player->lives--;
			}
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player *player, struct map *map)
{
	int x = player->x;
	int y = player->y;
	int move = 0;

	if (!map_is_inside(map, x, y))
		return 0;

	switch (player->direction)
	{
	case NORTH:
		if (player_move_aux(player, map, x, y - 1))
		{
			player->y--;
			move = 1;
		}
		break;

	case SOUTH:
		if (player_move_aux(player, map, x, y + 1))
		{
			player->y++;
			move = 1;
		}
		break;

	case WEST:
		if (player_move_aux(player, map, x - 1, y))
		{
			player->x--;
			move = 1;
		}
		break;

	case EAST:
		if (player_move_aux(player, map, x + 1, y))
		{
			player->x++;
			move = 1;
		}
		break;
	}

	if (move)
	{
		if (map_get_cell_type(map, x, y) != CELL_BOMB && map_get_cell_type(map, x, y) != CELL_DOOR && map_get_cell_type(map, x, y) != CELL_MONSTER)
		{
			map_set_cell_type(map, x, y, CELL_EMPTY);
		}
	}
	return move;
}

void player_display(struct player *player)
{
	assert(player);
	window_display_image(sprite_get_player(player->direction),
						 player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}
