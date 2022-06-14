#include <SDL/SDL_image.h>
#include <assert.h>
#include <unistd.h>
#include <bomb.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <map.h>
#include <game.h>
#include <player.h>

struct bomb
{
    int x_bomb;
    int y_bomb;
    int t_init;
    int range;
    struct player *player;
    struct bomb *next_bomb;
    struct bomb *prev_bomb;
};

struct bomb *bomb_init()
{
    struct bomb *bomb_ = malloc(sizeof(*bomb_));
    if (!bomb_)
    {
        error("Memory error");
    }
    assert(bomb_);
    bomb_->x_bomb = 0;
    bomb_->y_bomb = 0;
    bomb_->range = 1;
    bomb_->t_init = -1;
    bomb_->next_bomb = NULL;
    bomb_->prev_bomb = NULL;
    return bomb_;
}

int bomb_get_x(struct bomb *bomb)
{
    assert(bomb);
    return bomb->x_bomb;
}

int bomb_get_y(struct bomb *bomb)
{
    assert(bomb != NULL);
    return bomb->y_bomb;
}

int bomb_get_t(struct bomb *bomb)
{
    assert(bomb != NULL);
    return bomb->t_init;
}

struct bomb *place_bomb(struct bomb *bomb, struct game *game)
{
    assert(bomb);
    struct player *player = game_get_player(game);
    struct bomb *newbomb = malloc(sizeof(*bomb));

    int player_x = player_get_x(player);
    int player_y = player_get_y(player);

    newbomb->t_init = SDL_GetTicks();
    newbomb->x_bomb = player_x;
    newbomb->y_bomb = player_y;
    newbomb->next_bomb = bomb;

    if (player_get_nb_bomb(player) != 0)
    {
        player_dec_nb_bomb(player);
    }

    bomb->prev_bomb = newbomb;

    return newbomb;
}

void bomb_explose(struct game *game)
{
    struct bomb *bomb = game_get_bomb(game);
    struct map *map = game_get_current_map(game);
    struct player *player = game_get_player(game);
    struct monster *monster = game_get_monster(game);

    while (bomb->next_bomb != NULL)
    {
        int t = bomb->t_init;
        int x = bomb->x_bomb;
        int y = bomb->y_bomb;
        int timer = SDL_GetTicks();

        if (timer - t <= 1000)
        {
            map_set_bomb_type(map, x, y, CELL_BOMB_4);
        }
        else if (timer - t <= 2000 && timer - t > 1000)
        {
            map_set_bomb_type(map, x, y, CELL_BOMB_3);
        }
        else if (timer - t <= 3000 && timer - t > 2000)
        {
            map_set_bomb_type(map, x, y, CELL_BOMB_2);
        }
        else if (timer - t <= 4000 && timer - t > 3000)
        {
            map_set_bomb_type(map, x, y, CELL_BOMB_1);
        }
        else if (timer - t <= 5000 && timer - t > 4000)
        {
            if (map_get_compose_type(map, x, y) == CELL_BOMB_1)
            {
                map_set_compose_type(map, x, y, CELL_EXPLOSION);
            }

            if (map_is_inside(map, x + 1, y))
            {
                switch (map_get_cell_type(map, x + 1, y))
                {
                case CELL_EMPTY:
                    map_set_bomb_type(map, x + 1, y, CELL_EXPLOSION);
                    break;
                case CELL_BOX:
                    switch (map_get_compose_type(map, x + 1, y))
                    {
                    case CELL_BOX_BOMBDEC:
                        map_set_bonus_type(map, x + 1, y, CELL_BONUS_BOMBDEC);
                        break;
                    case CELL_BOX_BOMBINC:
                        map_set_bonus_type(map, x + 1, y, CELL_BONUS_BOMBINC);
                        break;
                    case CELL_BOX_RANGEDEC:
                        map_set_bonus_type(map, x + 1, y, CELL_BONUS_RANGEDEC);
                        break;
                    case CELL_BOX_RANGEINC:
                        map_set_bonus_type(map, x + 1, y, CELL_BONUS_RANGEINC);
                        break;
                    case CELL_BOX_LIFE:
                        map_set_bonus_type(map, x + 1, y, CELL_BONUS_LIFE);
                        break;
                    case CELL_BOX_MONSTER:
                        spawn_monster(monster, x + 1, y);
                        break;
                    default:
                        break;
                    }
                    break;
                case CELL_KEY:
                    map_set_bomb_type(map, x + 1, y, CELL_EXPLOSION);
                    break;
                default:
                    break;
                }
            }
            if (map_is_inside(map, x - 1, y))
            {
                switch (map_get_cell_type(map, x - 1, y))
                {
                case CELL_EMPTY:
                    map_set_bomb_type(map, x - 1, y, CELL_EXPLOSION);
                    break;
                case CELL_BOX:
                    switch (map_get_compose_type(map, x - 1, y))
                    {
                    case CELL_BOX_BOMBDEC:
                        map_set_bonus_type(map, x - 1, y, CELL_BONUS_BOMBDEC);
                        break;
                    case CELL_BOX_BOMBINC:
                        map_set_bonus_type(map, x - 1, y, CELL_BONUS_BOMBINC);
                        break;
                    case CELL_BOX_RANGEDEC:
                        map_set_bonus_type(map, x - 1, y, CELL_BONUS_RANGEDEC);
                        break;
                    case CELL_BOX_RANGEINC:
                        map_set_bonus_type(map, x - 1, y, CELL_BONUS_RANGEINC);
                        break;
                    case CELL_BOX_LIFE:
                        map_set_bonus_type(map, x - 1, y, CELL_BONUS_LIFE);
                        break;
                    case CELL_BOX_MONSTER:
                        spawn_monster(monster, x, y - 1);
                        break;
                    default:
                        break;
                    }
                    break;
                case CELL_KEY:
                    map_set_bomb_type(map, x - 1, y, CELL_EXPLOSION);
                    break;

                default:
                    break;
                }
            }
            if (map_is_inside(map, x, y + 1))
            {
                switch (map_get_cell_type(map, x, y + 1))
                {
                case CELL_EMPTY:
                    map_set_bomb_type(map, x, y + 1, CELL_EXPLOSION);
                    break;
                case CELL_BOX:
                    switch (map_get_compose_type(map, x, y + 1))
                    {
                    case CELL_BOX_BOMBDEC:
                        map_set_bonus_type(map, x, y + 1, CELL_BONUS_BOMBDEC);
                        break;
                    case CELL_BOX_BOMBINC:
                        map_set_bonus_type(map, x, y + 1, CELL_BONUS_BOMBINC);
                        break;
                    case CELL_BOX_RANGEDEC:
                        map_set_bonus_type(map, x, y + 1, CELL_BONUS_RANGEDEC);
                        break;
                    case CELL_BOX_RANGEINC:
                        map_set_bonus_type(map, x, y + 1, CELL_BONUS_RANGEINC);
                        break;
                    case CELL_BOX_LIFE:
                        map_set_bonus_type(map, x, y + 1, CELL_BONUS_LIFE);
                        break;
                    case CELL_BOX_MONSTER:
                        spawn_monster(monster, x, y + 1);
                        break;
                    default:
                        break;
                    }
                    break;
                case CELL_KEY:
                    map_set_bomb_type(map, x, y + 1, CELL_EXPLOSION);
                    break;
                default:
                    break;
                }
            }
            if (map_is_inside(map, x, y - 1))
            {
                switch (map_get_cell_type(map, x, y - 1))
                {
                case CELL_EMPTY:
                    map_set_bomb_type(map, x, y - 1, CELL_EXPLOSION);
                    break;
                case CELL_BOX:
                    switch (map_get_compose_type(map, x, y - 1))
                    {
                    case CELL_BOX_BOMBDEC:
                        map_set_bonus_type(map, x, y - 1, CELL_BONUS_BOMBDEC);
                        break;
                    case CELL_BOX_BOMBINC:
                        map_set_bonus_type(map, x, y - 1, CELL_BONUS_BOMBINC);
                        break;
                    case CELL_BOX_RANGEDEC:
                        map_set_bonus_type(map, x, y - 1, CELL_BONUS_RANGEDEC);
                        break;
                    case CELL_BOX_RANGEINC:
                        map_set_bonus_type(map, x, y - 1, CELL_BONUS_RANGEINC);
                        break;
                    case CELL_BOX_LIFE:
                        map_set_bonus_type(map, x, y - 1, CELL_BONUS_LIFE);
                        break;
                    case CELL_BOX_MONSTER:
                        spawn_monster(monster, x, y - 1);
                        break;
                    default:
                        break;
                    }
                    break;
                case CELL_KEY:
                    map_set_bomb_type(map, x, y - 1, CELL_EXPLOSION);
                    break;
                default:
                    break;
                }
            }
        }
        else if (timer - t <= 6000 && timer - t > 5000 && player_get_nb_bomb(player) > 0)
        {
            enum compose_type center = map_get_compose_type(map, x, y);
            enum compose_type east = map_get_compose_type(map, x + 1, y);
            enum compose_type west = map_get_compose_type(map, x - 1, y);
            enum compose_type north = map_get_compose_type(map, x, y + 1);
            enum compose_type south = map_get_compose_type(map, x, y - 1);

            if (center == CELL_EXPLOSION)
                map_set_cell_type(map, x, y, CELL_EMPTY);
            else if (east == CELL_EXPLOSION)
                map_set_cell_type(map, x + 1, y, CELL_EMPTY);
            else if (west == CELL_EXPLOSION)
                map_set_cell_type(map, x - 1, y, CELL_EMPTY);
            else if (north == CELL_EXPLOSION)
                map_set_cell_type(map, x, y + 1, CELL_EMPTY);
            else if (south == CELL_EXPLOSION)
                map_set_cell_type(map, x, y - 1, CELL_EMPTY);
        }

        bomb = bomb->next_bomb;
    }
}

void fire_propage(struct game *game)
{

    struct map *map = game_get_current_map(game);
    struct bomb *bomb = game_get_bomb(game);

    int x = bomb_get_x(bomb);
    int y = bomb_get_y(bomb);

    int r = bomb->range;

    int r_east = current_range_east(r, x, y, map);
    int r_west = current_range_west(r, x, y, map);
    int r_north = current_range_south(r, x, y, map);
    int r_south = current_range_north(r, x, y, map);

    if (map_get_compose_type(map, x, y) == CELL_BOMB_1)
    {
        map_set_bomb_type(map, x, y, CELL_EXPLOSION);
    }

    for (int i = 1; i <= r_east; i++)
    {
        if (map_is_inside(map, x + i, y))
        {
            switch (map_get_cell_type(map, x + i, y))
            {
            case CELL_EMPTY:
                map_set_bomb_type(map, x + i, y, CELL_EXPLOSION);
                break;
            case CELL_BOX:
                switch (map_get_compose_type(map, x + i, y))
                {
                case CELL_BOX_BOMBDEC:
                    map_set_bonus_type(map, x + i, y, CELL_BONUS_BOMBDEC);
                    break;
                case CELL_BOX_BOMBINC:
                    map_set_bonus_type(map, x + i, y, CELL_BONUS_BOMBINC);
                    break;
                case CELL_BOX_RANGEDEC:
                    map_set_bonus_type(map, x + i, y, CELL_BONUS_RANGEDEC);
                    break;
                case CELL_BOX_RANGEINC:
                    map_set_bonus_type(map, x + i, y, CELL_BONUS_RANGEINC);
                    break;
                case CELL_BOX_LIFE:
                    map_set_bonus_type(map, x + i, y, CELL_BONUS_LIFE);
                    break;
                default:
                    break;
                }
                break;
            case CELL_BONUS:
                map_set_bomb_type(map, x + i, y, CELL_EXPLOSION);
                break;
            case CELL_KEY:
                map_set_bomb_type(map, x + i, y, CELL_EXPLOSION);
                break;
            default:
                break;
            }
        }
    }
    for (int i = 1; i <= r_south; i++)
    {
        if (map_is_inside(map, x, y + i))
        {
            switch (map_get_cell_type(map, x, y + i))
            {
            case CELL_EMPTY:
                map_set_bomb_type(map, x, y + i, CELL_EXPLOSION);
                break;
            case CELL_BOX:
                switch (map_get_compose_type(map, x, y + 1))
                {
                case CELL_BOX_BOMBDEC:
                    map_set_bonus_type(map, x, y + i, CELL_BONUS_BOMBDEC);
                    break;
                case CELL_BOX_BOMBINC:
                    map_set_bonus_type(map, x, y + i, CELL_BONUS_BOMBINC);
                    break;
                case CELL_BOX_RANGEDEC:
                    map_set_bonus_type(map, x, y + i, CELL_BONUS_RANGEDEC);
                    break;
                case CELL_BOX_RANGEINC:
                    map_set_bonus_type(map, x, y + i, CELL_BONUS_RANGEINC);
                    break;
                case CELL_BOX_LIFE:
                    map_set_bonus_type(map, x, y + i, CELL_BONUS_LIFE);
                    break;
                default:
                    break;
                }
                break;
            case CELL_BONUS:
                map_set_bomb_type(map, x, y + i, CELL_EXPLOSION);
                break;
            case CELL_KEY:
                map_set_bomb_type(map, x, y + i, CELL_EXPLOSION);
                break;
            default:
                break;
            }
        }
    }
    for (int i = 1; i <= r_west; i++)
    {
        if (map_is_inside(map, x - i, y))
        {
            switch (map_get_cell_type(map, x - i, y))
            {
            case CELL_EMPTY:
                map_set_bomb_type(map, x - i, y, CELL_EXPLOSION);
                break;
            case CELL_BOX:
                switch (map_get_compose_type(map, x - i, y))
                {
                case CELL_BOX_BOMBDEC:
                    map_set_bonus_type(map, x - i, y, CELL_BONUS_BOMBDEC);
                    break;
                case CELL_BOX_BOMBINC:
                    map_set_bonus_type(map, x - i, y, CELL_BONUS_BOMBINC);
                    break;
                case CELL_BOX_RANGEDEC:
                    map_set_bonus_type(map, x - i, y, CELL_BONUS_RANGEDEC);
                    break;
                case CELL_BOX_RANGEINC:
                    map_set_bonus_type(map, x - i, y, CELL_BONUS_RANGEINC);
                    break;
                case CELL_BOX_LIFE:
                    map_set_bonus_type(map, x - i, y, CELL_BONUS_LIFE);
                    break;
                default:
                    break;
                }
                break;
            case CELL_BONUS:
                map_set_bomb_type(map, x - i, y, CELL_EXPLOSION);
                break;
            case CELL_KEY:
                map_set_bomb_type(map, x - i, y, CELL_EXPLOSION);
                break;
            default:
                break;
            }
        }
    }
    for (int i = 1; i <= r_north; i++)
    {
        if (map_is_inside(map, x, y - i))
        {
            switch (map_get_cell_type(map, x, y - i))
            {
            case CELL_EMPTY:
                map_set_bomb_type(map, x, y - i, CELL_EXPLOSION);
                break;
            case CELL_BOX:
                switch (map_get_compose_type(map, x, y - 1))
                {
                case CELL_BOX_BOMBDEC:
                    map_set_bonus_type(map, x, y - i, CELL_BONUS_BOMBDEC);
                    break;
                case CELL_BOX_BOMBINC:
                    map_set_bonus_type(map, x, y - i, CELL_BONUS_BOMBINC);
                    break;
                case CELL_BOX_RANGEDEC:
                    map_set_bonus_type(map, x, y - i, CELL_BONUS_RANGEDEC);
                    break;
                case CELL_BOX_RANGEINC:
                    map_set_bonus_type(map, x, y - i, CELL_BONUS_RANGEINC);
                    break;
                case CELL_BOX_LIFE:
                    map_set_bonus_type(map, x, y - i, CELL_BONUS_LIFE);
                    break;
                default:
                    break;
                }
                break;
            case CELL_BONUS:
                map_set_bomb_type(map, x, y - i, CELL_EXPLOSION);
                break;
            case CELL_KEY:
                map_set_bomb_type(map, x, y - i, CELL_EXPLOSION);
                break;
            default:
                break;
            }
        }
    }
}

int current_range_east(int r, int x, int y, struct map *map)
{
    int j = 0;
    while (j < r)
    {
        if (map_get_cell_type(map, x + j, y) == CELL_EMPTY)
            j++;
    }
    r = j;
    return r;
}

int current_range_north(int r, int x, int y, struct map *map)
{
    int j = 0;
    while (j < r)
    {
        if (map_get_cell_type(map, x, y - j) == CELL_EMPTY)
            j++;
    }
    r = j;
    return r;
}

int current_range_west(int r, int x, int y, struct map *map)
{
    int j = 0;
    while (j < r)
    {
        if (map_get_cell_type(map, x - j, y) == CELL_EMPTY)
            j++;
    }
    r = j;
    return r;
}

int current_range_south(int r, int x, int y, struct map *map)
{

    int j = 0;
    while (j < r)
    {
        if (map_get_cell_type(map, x, y + j) == CELL_EMPTY)
            j++;
    }
    r = j;
    return r;
}
