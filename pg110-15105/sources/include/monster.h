#ifndef MONSTER_H_
#define MONSTER_H_

#include <map.h>
#include <constant.h>

struct monster;

struct monster* monster_init(int speed, int time);


// Set the position of the monster
void monster_set_position(struct monster *monster, int x, int y);

// Returns the current position of the monster
int monster_get_x(struct monster* monster);
int monster_get_y(struct monster* monster);

// Returns the speed of the monster
int monster_get_speed(struct monster* monster);

// Returns the last time the monster moved
int monster_get_time(struct monster* monster);

// Set the time the monster last moved
void monster_set_time(struct monster* monster, int time);

// Set the direction of the next move of the monster
void monster_set_current_way(struct monster * monster, enum direction direction);

// Move the monster according to the current direction
int monster_move(struct monster* monster, struct map* map);

// Display the monster on the screen
void monster_display(struct monster* monster); 

// free monster
void monster_free(struct monster* monster);

// Fait apparaitre un monstre
void spawn_monster(struct monster* monster, int x, int y);

#endif /* MONSTER_H_ */
