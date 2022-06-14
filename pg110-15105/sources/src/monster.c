#include <SDL/SDL_image.h>
#include <assert.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <map.h>
#include <monster.h>



struct monster {
	int x_monster, y_monster, speed_monster, last_time;
	enum direction direction_monster;
};

struct monster* monster_init(int speed, int time) {
	struct monster* monster = malloc(sizeof(*monster));
	if (!monster){
		error("Memory error");
		}
	monster->last_time = time;
	monster->direction_monster = SOUTH;
	monster->speed_monster = speed;

	return monster;
}

void spawn_monster(struct monster* monster, int x, int y) {          // pourrait être utile pour faire apparaître plusieurs monstres
		assert(monster);
		monster->x_monster = x;
		monster->y_monster = y;
}

void monster_set_position(struct monster *monster, int x, int y) {
	assert(monster);
	monster->x_monster = x;
	monster->y_monster = y;
}


void monster_free(struct monster* monster) {
	assert(monster);
	free(monster);
}

int monster_get_x(struct monster* monster) {
	assert(monster != NULL);
	return monster->x_monster;
}

int monster_get_y(struct monster* monster) {
	assert(monster != NULL);
	return monster->y_monster;
}
int monster_get_time(struct monster* monster) {
	assert(monster != NULL);
	return monster->last_time;
}

void monster_set_time(struct monster* monster, int time) {
	assert(monster != NULL);
	monster->last_time = time;
}

int monster_get_speed(struct monster* monster) {
	assert(monster != NULL);
	return monster->speed_monster;	
}

void monster_set_current_way(struct monster* monster, enum direction way) {
	assert(monster);
	monster->direction_monster = way;
}

static int monster_move_aux(struct monster* monster, struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY: // prevent the monster from walking on a rock or a tree
		return 0;
		break;

	case CELL_BOX: // must allow the monster to push a single box + keeping the box inside the boudaries of the map
	/*switch (monster->direction_monster) {
	case NORTH:
		if ((map_is_inside(map, x, y - 1)) && (map_get_cell_type(map, x, y - 1) == CELL_EMPTY)) { // the order of the test is important so the game won't crash
			map_set_cell_type(map, x, y, CELL_EMPTY);
			map_set_cell_type(map, x, y - 1, CELL_BOX);
			return 1;
		}
		break;
	case SOUTH:
		if ((map_is_inside(map, x, y + 1)) && (map_get_cell_type(map, x, y + 1) == CELL_EMPTY)) {
			map_set_cell_type(map, x, y, CELL_EMPTY);
			map_set_cell_type(map, x, y + 1, CELL_BOX);
			return 1;
		}
		break;
	case EAST:
		if ((map_is_inside(map, x + 1, y)) && (map_get_cell_type(map, x + 1, y) == CELL_EMPTY)) {
			map_set_cell_type(map, x, y, CELL_EMPTY);
			map_set_cell_type(map, x + 1, y, CELL_BOX);
			return 1;
		}
		break;
	case WEST:
		if ((map_is_inside(map, x - 1, y)) && (map_get_cell_type(map, x - 1, y) == CELL_EMPTY)) {
			map_set_cell_type(map, x, y, CELL_EMPTY);
			map_set_cell_type(map, x - 1, y, CELL_BOX);
			return 1;
		}
		break;
	}
	*/
		return 0;
		break;

	case CELL_BONUS: // the monster can't pick up a bonus and is blocked by it for now
		return 0;
		break;

	case CELL_MONSTER: // monsters can't walk past each other
		return 0;
		break;

	case CELL_DOOR:
		return 0;
		break;

	case CELL_KEY: // monster can't pick up a key
		return 0;
		break;

	case CELL_BOMB: // monsters can't walk on bombs
		return 0;
		break;

	default:
		break;
	}

	// monster has moved
	return 1;
}

int monster_move(struct monster* monster, struct map* map) {
	int x = monster->x_monster;
	int y = monster->y_monster;
	/*int cond = player_get_condition(player);
	int lf = player_get_nb_lives(player);
	int i = player_get_x(player);
	int j = player_get_y(player);*/
	int move = 0;


	if (!map_is_inside(map, x, y))
		return 0;

	switch (monster->direction_monster) {
	case NORTH:
		if (monster_move_aux(monster, map, x, y - 1)) {
			monster->y_monster--;
			move = 1;
		}
		break;

	case SOUTH:
		if (monster_move_aux(monster, map, x, y + 1)) {
			monster->y_monster++;
			move = 1;
		}
		break;

	case WEST:
		if (monster_move_aux(monster, map, x - 1, y)) {
			monster->x_monster--;
			move = 1;
		}
		break;

	case EAST:
		if (monster_move_aux(monster, map, x + 1, y)) {
			monster->x_monster++;
			move = 1;
		}
		break;
	}
	map_set_cell_type(map, monster->x_monster, monster->y_monster, CELL_MONSTER);

	if (move)
	{
		if (map_get_cell_type(map, x, y) != CELL_BOMB && map_get_cell_type(map, x, y) != CELL_DOOR && map_get_cell_type(map, x, y) != CELL_KEY)
		{
			map_set_cell_type(map, x, y, CELL_EMPTY);
		}
	}
	return move;
}

void monster_display(struct monster* monster) {
	assert(monster);
	window_display_image(sprite_get_monster(monster->direction_monster), monster->x_monster * SIZE_BLOC, monster->y_monster * SIZE_BLOC);
}
