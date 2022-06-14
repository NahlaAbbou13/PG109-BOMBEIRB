/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef MAP_H_
#define MAP_H_

enum cell_type
{
	CELL_EMPTY = 0x00,	 //  0000 0000
	CELL_SCENERY = 0x10, //  0001 0000
	CELL_BOX = 0x20,	 //  0010 0000
	CELL_DOOR = 0x30,	 //  0011 0000
	CELL_KEY = 0x40,	 //  0100 0000
	CELL_BONUS = 0x50,	 // 	0101 0000
	CELL_MONSTER = 0x60, // 	0110 0000
	CELL_BOMB = 0x70	 // 	0111 0000
};

enum bomb_type
{
	BOMB_1 = 1,
	BOMB_2 = 2,
	BOMB_3 = 3,
	BOMB_4 = 4,
	EXPLOSE = 5
};

enum bonus_type
{
	BONUS_BOMB_RANGE_DEC = 1,
	BONUS_BOMB_RANGE_INC,
	BONUS_BOMB_NB_DEC,
	BONUS_BOMB_NB_INC,
	BONUS_MONSTER,
	BONUS_LIFE
};

enum scenery_type
{
	SCENERY_STONE = 7, // 0001
	SCENERY_TREE,	   // 0010
	SCENERY_PRINCESS   // 0100
};

enum door_type
{
	DOOR_OPENED,
	DOOR_CLOSED,
};

enum compose_type
{
	CELL_TREE = CELL_SCENERY | SCENERY_TREE,
	CELL_STONE = CELL_SCENERY | SCENERY_STONE,
	CELL_PRINCESS = CELL_SCENERY | SCENERY_PRINCESS,
	CELL_OPEN = CELL_DOOR | DOOR_OPENED,
	CELL_CLOSE = CELL_DOOR | DOOR_CLOSED,

	CELL_BOX_RANGEINC = CELL_BOX | BONUS_BOMB_RANGE_INC,
	CELL_BOX_RANGEDEC = CELL_BOX | BONUS_BOMB_RANGE_DEC,
	CELL_BOX_BOMBINC = CELL_BOX | BONUS_BOMB_NB_INC,
	CELL_BOX_BOMBDEC = CELL_BOX | BONUS_BOMB_NB_DEC,
	CELL_BOX_LIFE = CELL_BOX | BONUS_LIFE,
	CELL_BOX_MONSTER = CELL_BOX | BONUS_MONSTER,

	CELL_BONUS_RANGEINC = CELL_BONUS | BONUS_BOMB_RANGE_INC,
	CELL_BONUS_RANGEDEC = CELL_BONUS | BONUS_BOMB_RANGE_DEC,
	CELL_BONUS_BOMBINC = CELL_BONUS | BONUS_BOMB_NB_DEC,
	CELL_BONUS_BOMBDEC = CELL_BONUS | BONUS_BOMB_NB_INC,
	CELL_BONUS_LIFE = CELL_BONUS | BONUS_LIFE,
	CELL_BONUS_MONSTER = CELL_BONUS | BONUS_MONSTER,

	CELL_BOMB_1 = CELL_BOMB | BOMB_1,
	CELL_BOMB_2 = CELL_BOMB | BOMB_2,
	CELL_BOMB_3 = CELL_BOMB | BOMB_3,
	CELL_BOMB_4 = CELL_BOMB | BOMB_4,
	CELL_EXPLOSION = CELL_BOMB | EXPLOSE,
};

enum monster_type
{
	MONSTER_UP,
	MONSTER_DOWN,
	MONSTER_LEFT,
	MONSTER_RIGHT,
};

struct map;

// Create a new empty map
struct map *map_new(int width, int height);
void map_free(struct map *map);

// Return the height and width of a map
int map_get_width(struct map *map);
int map_get_height(struct map *map);

// Return the type of a cell
enum cell_type map_get_cell_type(struct map *map, int x, int y);

// Get the type of a bomb
enum bomb_type map_get_bomb_type(struct map *map, int x, int y);

enum compose_type map_get_compose_type(struct map *map, int x, int y);

// Set the type of a cell
void map_set_cell_type(struct map *map, int x, int y, enum cell_type type);

void map_set_bomb_type(struct map *map, int x, int y, enum bomb_type type);

// Return the type of the door
enum door_type map_get_door_type(struct map *map, int x, int y);

// Set the type of a door
void map_set_door_type(struct map *map, int x, int y, enum door_type type);

void map_set_compose_type(struct map *map, int x, int y, enum compose_type type);

void map_set_bonus_type(struct map *map, int x, int y, enum bonus_type type);

enum compose_type map_get_compose_type(struct map *map, int x, int y);

// Test if (x,y) is within the map
int map_is_inside(struct map *map, int x, int y);

// Return a default static map
struct map *map_get__level(char level);

// Display the map on the screen
void map_display(struct map *map);

void display_bomb(struct map *map, int x, int y, unsigned char type);

void display_door(struct map *map, int x, int y, unsigned char type);

#endif /* MAP_H_ */
