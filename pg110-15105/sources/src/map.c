/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <player.h>
#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>

struct map
{
	int width;
	int height;
	int level;
	unsigned char *grid;
};

#define CELL(i, j) ((i) + (j)*map->width)

struct map *map_new(int width, int height)
{
	assert(width > 0 && height > 0);

	struct map *map = malloc(sizeof *map);
	if (map == NULL)
		error("map_new : malloc map failed");

	map->width = width;
	map->height = height;
	map->level = 0;

	map->grid = malloc(height * width);
	if (map->grid == NULL)
	{
		error("map_new : malloc grid failed");
	}

	// Grid cleaning
	int i, j;
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
			map->grid[CELL(i, j)] = CELL_EMPTY;

	return map;
}

int map_is_inside(struct map *map, int x, int y)
{
	if ((x >= 0) && (x <= map->width - 1) && (y >= 0) && (y <= map->height - 1))
	{
		return 1;
	}
	assert(map);
	return 0;
}

void map_free(struct map *map)
{
	if (map == NULL)
		return;
	free(map->grid);
	free(map);
}

int map_get_width(struct map *map)
{
	assert(map != NULL);
	return map->width;
}

int map_get_height(struct map *map)
{
	assert(map);
	return map->height;
}

enum cell_type map_get_cell_type(struct map *map, int x, int y)
{
	assert(map || map_is_inside(map, x, y));
	return map->grid[CELL(x, y)] & 0xf0;
}

enum compose_type map_get_compose_type(struct map *map, int x, int y)
{
	assert(map || map_is_inside(map, x, y));
	return map->grid[CELL(x, y)] & 0xff;
}

void map_set_cell_type(struct map *map, int x, int y, enum cell_type type)
{
	assert(map || map_is_inside(map, x, y));
	map->grid[CELL(x, y)] = type;
}

void map_set_compose_type(struct map *map, int x, int y, enum compose_type type)
{
	assert(map || map_is_inside(map, x, y));
	map->grid[CELL(x, y)] = type;
}

void map_set_bomb_type(struct map *map, int x, int y, enum bomb_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x, y)] = type;
}

void map_set_bonus_type(struct map *map, int x, int y, enum bonus_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x, y)] = type;
}

// pour la porte

void map_set_door_type(struct map *map, int x, int y, enum door_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x, y)] = (CELL_DOOR | type);
}

enum door_type map_get_door_type(struct map *map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x, y)] & 0x0f;
}

void display_door(struct map *map, int x, int y, unsigned char type)
{
	switch (type & 0x0f)
	{ // sub-types are encoded with the 4 less significant bits
	case DOOR_OPENED:
		window_display_image(sprite_get_door_opened(), x, y);
		break;

	case DOOR_CLOSED:
		window_display_image(sprite_get_door_closed(), x, y);
		break;
	}
}

// fin porte

void display_bonus(struct map *map, int x, int y, unsigned char type)
{
	// bonus is encoded with the 4 most significant bits
	switch (type & 0x0f)
	{
	case BONUS_BOMB_RANGE_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
		break;

	case BONUS_BOMB_RANGE_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_DEC), x, y);
		break;

	case BONUS_BOMB_NB_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
		break;

	case BONUS_LIFE:
		window_display_image(sprite_get_bonus(BONUS_LIFE), x, y);
	}
}

void display_scenery(struct map *map, int x, int y, unsigned char type)
{
	switch (type & 0x0f)
	{ // sub-types are encoded with the 4 less significant bits
	case SCENERY_STONE:
		window_display_image(sprite_get_stone(), x, y);
		break;

	case SCENERY_TREE:
		window_display_image(sprite_get_tree(), x, y);
		break;
	}
}

void display_monster(struct map *map, int x, int y, unsigned char type)
{
	switch (type & 0x0f)
	{
	case MONSTER_UP:
		window_display_image(sprite_get_monster(NORTH), x, y);
		break;
	case MONSTER_DOWN:
		window_display_image(sprite_get_monster(SOUTH), x, y);
		break;
	case MONSTER_LEFT:
		window_display_image(sprite_get_monster(WEST), x, y);
		break;
	case MONSTER_RIGHT:
		window_display_image(sprite_get_monster(EAST), x, y);
		break;
	}
}

// debut bombe

enum bomb_type map_get_bomb_type(struct map *map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x, y)] & 0xf0;
}

void display_bomb(struct map *map, int x, int y, unsigned char type)
{
	switch (type & 0x0f)
	{
	case BOMB_1:
		window_display_image(sprite_get_bomb(1), x, y);
		break;
	case BOMB_2:
		window_display_image(sprite_get_bomb(2), x, y);
		break;
	case BOMB_3:
		window_display_image(sprite_get_bomb(3), x, y);
		break;
	case BOMB_4:
		window_display_image(sprite_get_bomb(4), x, y);
		break;
	case EXPLOSE:
		window_display_image(sprite_get_bomb(0), x, y);
	}
}

// fin bombe

void map_display(struct map *map)
{
	assert(map != NULL);
	assert(map->height > 0 && map->width > 0);

	int x, y;
	for (int i = 0; i < map->width; i++)
	{
		for (int j = 0; j < map->height; j++)
		{
			x = i * SIZE_BLOC;
			y = j * SIZE_BLOC;

			unsigned char type = map->grid[CELL(i, j)];

			switch (type & 0xf0)
			{
			case CELL_SCENERY:
				display_scenery(map, x, y, type);
				break;
			case CELL_BOX:
				window_display_image(sprite_get_box(), x, y);
				break;
			case CELL_BONUS:
				display_bonus(map, x, y, type);
				break;
			case CELL_KEY:
				window_display_image(sprite_get_key(), x, y);
				break;
			case CELL_DOOR:
				display_door(map, x, y, type);
				break;
			case CELL_BOMB:;
				// enum bomb_type b_type = map_get_bomb_type(map, i, j);
				display_bomb(map, x, y, type);
				break;
			case CELL_MONSTER:
				// display_monster(map,x, y, type);
				// window_display_image(sprite_get_monster(SOUTH), x, y);
				break;
			}
		}
	}
}

struct map *map_get__level(char level)
{
	int height, width;
	char fic_name[11] = "map_N/map_N";
	fic_name[10] = level;

	FILE *fic = fopen(fic_name, "r");
	fscanf(fic, "%d:%d", &width, &height);

	struct map *map = map_new(width, height);

	int x, y, c;
	for (x = 0; x < width; x++)
	{
		for (y = 0; y < height; y++)
		{
			int i = CELL(y, x);
			fscanf(fic, "%d_", &c);
			map->grid[i] = c;
		}
	}
	fclose(fic);
	return map;
}
