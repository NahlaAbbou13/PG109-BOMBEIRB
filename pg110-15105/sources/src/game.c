/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>
#include <stdlib.h>
#include <map.h>
#include <monster.h>
#include <player.h>
#include <bomb.h>

struct game
{
	struct map **map; // the game's map
	short levels;	  // nb map of the game
	short level;
	struct player *player;
	struct monster *monster;
	struct bomb *bomb;
};

struct game *game_new(void)
{
	sprite_load(); // load sprites into process memory

	struct game *game = malloc(sizeof(*game));
	game->map = malloc(sizeof(struct game));
	game->map[0] = map_get__level('1');
	game->map[1] = map_get__level('2');
	game->map[2] = map_get__level('3');
	game->map[3] = map_get__level('4');
	game->map[4] = map_get__level('5');
	game->levels = 5;
	game->level = 0;
	game->bomb = bomb_init();
	game->player = player_init(3, 3);
	game->monster = monster_init(1, 0); // set the speed of the monsters
	// Set default location of the player
	player_set_position(game->player, 1, 0);
	spawn_monster(game->monster, 4, 4);
	return game;
}

void game_free(struct game *game)
{
	assert(game);

	player_free(game->player);
	monster_free(game->monster);
	for (int i = 0; i < game->levels; i++)
		map_free(game->map[i]);
}

// Maps management

// Retrieve previous map
struct map *game_get_prev_map(struct game *game)
{
	assert(game);
	return game->map[game->level - 1];
}
// Retrieve current map
struct map *game_get_current_map(struct game *game)
{
	assert(game);
	return game->map[game->level];
}
// Retrieve next map
struct map *game_get_next_map(struct game *game)
{
	assert(game);
	return game->map[game->level + 1];
}

// Retrieve the player object of game structure
struct player *game_get_player(struct game *game)
{
	assert(game);
	return game->player;
}

short game_get_current_level(struct game *game)
{
	assert(game);
	return game->level;
}
// Retrieve the bomb object of game structure
struct bomb *game_get_bomb(struct game *game)
{
	assert(game);
	return game->bomb;
}

// Retrieve the monster object of game structure
struct monster *game_get_monster(struct game *game)
{
	assert(game);
	return game->monster;
}

// Information panel management

void game_banner_display(struct game *game)
{
	assert(game);

	struct map *map = game_get_current_map(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 11 * SIZE_BLOC) / 4;
	int x = white_bloc;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;

	// Display the number of lives
	window_display_image(sprite_get_banner_life(), x, y);
	x = white_bloc + SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_nb_lives(game->player)), x, y);

	// Display the number of bombs in the inventory
	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_bomb(1), x, y);
	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(
		sprite_get_number(player_get_nb_bomb(game_get_player(game)) - 1), x, y);

	// Display the range of bombs
	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);
	x = 3 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_rg_bomb(game_get_player(game))), x, y); // Default initialized to 1

	// Display the number of keys in the inventory
	x = 4 * white_bloc + 6 * SIZE_BLOC;
	window_display_image(sprite_get_key(), x, y);
	x = 4 * white_bloc + 7 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_key(game->player)), x, y);

	// Display the game level
	x = 4 * white_bloc + 10 * SIZE_BLOC;
	window_display_image(sprite_get_number(game_get_current_level(game) + 1), x, y); // Default initialized to 1
}

void game_display(struct game *game)
{
	assert(game);

	window_clear();
	game_banner_display(game);
	map_display(game_get_current_map(game));
	player_display(game->player);
	monster_display(game->monster);
	window_refresh();
	bomb_explose(game);
}

static short input_keyboard(struct game *game)
{
	SDL_Event event;
	struct player *player = game_get_player(game);
	struct map *map = game_get_current_map(game);
	int x = player_get_x(player);
	int y = player_get_y(player);
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				return 1;
			case SDLK_UP:
				player_set_current_way(player, NORTH);
				player_move(player, map);
				break;
			case SDLK_DOWN:
				player_set_current_way(player, SOUTH);
				player_move(player, map);
				break;
			case SDLK_RIGHT:
				player_set_current_way(player, EAST);
				player_move(player, map);
				break;
			case SDLK_LEFT:
				player_set_current_way(player, WEST);
				player_move(player, map);
				break;
			case SDLK_SPACE:
				if (player_get_nb_bomb(player) >= 2)
				{
					game->bomb = place_bomb(game->bomb, game);
				}
				break;
			case SDLK_p:
				game_break();
				break;
			case SDLK_RETURN:
				if ((x < 6 && y < 6) || (x > 6 && y > 6))
				{
					if (map_get_cell_type(map, x, y) == CELL_DOOR)
					{
						if (map_get_door_type(map, x, y) == DOOR_OPENED)
						{
							game->level++;
							switch(game->level)
							{
								case 0:
									spawn_monster(game->monster, 10, 1);
									player_set_position(game->player, 1, 11);
									break;
								case 1:
									spawn_monster(game->monster, 10, 10);
									player_set_position(game->player, 11, 0);
									break;
								case 2:
									spawn_monster(game->monster, 1, 1);
									player_set_position(game->player, 1, 11);
									break;
								case 3:
									spawn_monster(game->monster, 5, 9);
									player_set_position(game->player, 4, 11);
									break;
								case 4:
									spawn_monster(game->monster, 9, 9);
									player_set_position(game->player, 1, 11);
									break;
							}
						}
					}
				}
				if ((x >= 6 && y <= 6) || (x <= 6 && y >= 6))
				{
					if (map_get_cell_type(map, x, y) == CELL_DOOR)
					{
						if (map_get_door_type(map, x, y) == DOOR_OPENED)
						{
							game->level--;
							switch(game->level)
							{
								case 0:
									spawn_monster(game->monster, 10, 1);
									player_set_position(game->player, 9, 11);
									break;
								case 1:
									spawn_monster(game->monster, 10, 10);
									player_set_position(game->player, 0, 4);
									break;
								case 2:
									spawn_monster(game->monster, 1, 1);
									player_set_position(game->player, 4, 0);
									break;
								case 3:
									spawn_monster(game->monster, 5, 9);
									player_set_position(game->player, 0, 0);
									break;
								case 4:
									spawn_monster(game->monster, 9, 9);
									player_set_position(game->player, 2, 12);
									break;
							}
						}
					}
				}
				break;
			default:
				break;
			}

			break;
		}
	}
	return 0;
}

void game_break()
{
	SDL_Event event;
	int game_status = 1;
	while (game_status)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_p:
				game_status = 0;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

int monster_movement(struct game *game, int z)
{
	struct map *map = game_get_current_map(game);


	struct monster *monster = game_get_monster(game);

	switch (z)
	{
	case 0:
		monster_set_current_way(monster, NORTH);
		monster_move(monster, map);
		break;
	case 1:
		monster_set_current_way(monster, SOUTH);
		monster_move(monster, map);
		break;
	case 2:
		monster_set_current_way(monster, EAST);
		monster_move(monster, map);
		break;
	case 3:
		monster_set_current_way(monster, WEST);
		monster_move(monster, map);
		break;
	default:
		break;
	}
	//}
	//}
	//}
	return 0;
}

int game_update(struct game *game)
{
	int speed_m = monster_get_speed(game->monster);
	int last_m = monster_get_time(game->monster);
	int delay_m = 500 * speed_m;

	int last_p = player_get_time(game->player);

	if ((last_m + delay_m) <= SDL_GetTicks())
	{
		int num = rand() % 4;
		monster_movement(game, num);
		monster_set_time(game->monster, SDL_GetTicks());
	}

	if (SDL_GetTicks() - last_p >= 3000)
	{
		player_set_condition(game->player, 0);
	}

	if (input_keyboard(game))
		return 1; // exit game

	return 0;
}
