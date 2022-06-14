#include <SDL/SDL_image.h>
#include <assert.h>
#include <map.h>
#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>

struct bomb;

int bomb_get_x(struct bomb *bomb);
int bomb_get_y(struct bomb *bomb);

struct bomb *place_bomb(struct bomb *bomb, struct game *game);
void bomb_explose(struct game *game);
struct bomb *bomb_init();

void fire_propage(struct game *game);
int current_range_east(int r, int x, int y, struct map *map);
int current_range_west(int r, int x, int y, struct map *map);
int current_range_south(int r, int x, int y, struct map *map);
int current_range_north(int r, int x, int y, struct map *map);
