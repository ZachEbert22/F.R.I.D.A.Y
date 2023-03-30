//
// Created by Andrew Bowie on 3/29/23.
//

#include "dragon_maze.h"
#include "stdbool.h"
#include "stdio.h"
#include "hash_map.h"
#include "linked_list.h"
#include "math.h"
#include "memory.h"
#include "mpx/heap.h"
#include "string.h"

#define EMPTY ' '
#define FOUR_WAY_WALL '+'
#define HORIZONTAL_WALL '-'
#define VERTICAL_WALL '|'
#define FINISH 'F'
#define PRINCESS 'P'
#define DRAGON 'D'
#define HERO_WITH_PRINCESS 'H'
#define HERO_NO_PRINCESS 'h'

///The default length of the maze.
#define MAZE_LENGTH 21
///The default height of the maze.
#define MAZE_HEIGHT 11

///An enum representing direction.
typedef enum {
    ///Represents the 'up' direction.
    W,
    ///Represents the 'left' direction.
    A,
    ///Represents the 'down' direction.
    S,
    ///Represents the 'right' direction.
    D,
} direction_t;

///A struct for a 2d coordinate
typedef struct
{
    ///The y coordinate
    int x;
    ///The z coordinate
    int y;
} coordinate_t;

/**
 * Shifts the given coordinate by the given direction.
 *
 * @param coordinate the coordinate.
 * @param direction the direction.
 * @param multi the multiplier of shift.
 * @return the shifted coordinate.
 */
coordinate_t shift(coordinate_t coordinate, direction_t direction, int multi)
{
    coordinate.x += (direction == A ? -1 : direction == D ? 1 : 0) * multi;
    coordinate.y += (direction == W ? -1 : direction == S ? 1 : 0) * multi;
    return coordinate;
}

/**
 * Compares equality between the two coordinates.
 *
 * @param c1 the first coordinate.
 * @param c2 the second coordinate.
 * @return true if the coordinates are equal, false if not.
 */
bool coordinate_eq(coordinate_t *c1, coordinate_t *c2)
{
    return c1->x == c2->x && c1->y == c2->y;
}

/**
 * Hashes the given coordinate.
 *
 * @param coordinate the coordinate to hash.
 * @return the hash of the coordinate.
 */
int coordinate_hash(coordinate_t *coordinate)
{
    return (coordinate->x * 31 + coordinate->y * 31) * 31;
}

///The maze board struct.
typedef struct
{
    ///The pieces that make up the board.
    char board_pieces[MAZE_HEIGHT][MAZE_LENGTH];
} maze_board_t;

///Denotes if the dragon is still alive.
static bool dragon_alive = false;
///The active maze board for the current run of the game.
static maze_board_t board;

///The map to use for visited tiles in maze generation.
static bool visited_map[MAZE_HEIGHT][MAZE_LENGTH] = {0};
/**
 * @brief The third, and final, step of board generation. Creates the paths through the maze.
 *
 * @param coordinate the coordinates.
 * @param end_points the end points list.
 */
void check_location(coordinate_t coordinate, linked_list *end_points)
{
    linked_list *queue = nl_unbounded();
    add_item(queue, (void *) W);
    add_item(queue, (void *) A);
    add_item(queue, (void *) S);
    add_item(queue, (void *) D);

    bool found = false;
    while(queue->_size > 0)
    {
        direction_t direc = (direction_t) remove_item_unsafe(queue, (int) next_random_lim(queue->_size));
        coordinate_t new_visit = shift(coordinate, direc, 2);

        if(new_visit.x < 0 || new_visit.x >= MAZE_LENGTH ||
                new_visit.y < 0 || new_visit.y >= MAZE_HEIGHT || visited_map[new_visit.y][new_visit.x])
            continue;

        //Check if the connection location is on the edge.
        coordinate_t connection_loc = shift(coordinate, direc, 1);
        if(connection_loc.x == 0 || connection_loc.y == 0 || connection_loc.x + 1 == MAZE_LENGTH || connection_loc.y + 1 == MAZE_HEIGHT)
            continue;

        //Get the piece.
        char piece = board.board_pieces[connection_loc.y][connection_loc.x];
        if(piece == EMPTY)
            continue;

        found = true;
        visited_map[new_visit.y][new_visit.x] = true;
        board.board_pieces[connection_loc.y][connection_loc.x] = EMPTY;

        //Recursively continue.
        check_location(new_visit, end_points);
    }

    if(!found)
    {
        coordinate_t *alloc_coord = sys_alloc_mem(sizeof(coordinate_t));
        alloc_coord->x = coordinate.x;
        alloc_coord->y = coordinate.y;
        add_item(end_points, alloc_coord);
    }

    ll_clear_free(queue, false);
    sys_free_mem(queue);
}

/**
 * @brief The second step of board generation. Controls the depth first generation of the paths
 * and places the hero, dragon, and princess when complete.
 */
void fill_randomly()
{
    linked_list *list = nl_unbounded();

    coordinate_t *origin = sys_alloc_mem(sizeof (coordinate_t));
    origin->x = origin->y = 1;

    while(list->_size < 3)
    {
        memset(visited_map, 0, sizeof(visited_map));
        ll_clear_free(list, true);

        check_location(*origin, list);
    }

    //Get all the points for hero, dragon, and princess.
    coordinate_t *hero_point = remove_item_unsafe(list, (int) next_random_lim(list->_size));
    coordinate_t *dragon_point = remove_item_unsafe(list, (int) next_random_lim(list->_size));
    coordinate_t *princess_point = remove_item_unsafe(list, (int) next_random_lim(list->_size));

    board.board_pieces[hero_point->y][hero_point->x] = HERO_NO_PRINCESS;
    board.board_pieces[dragon_point->y][dragon_point->x] = DRAGON;
    board.board_pieces[princess_point->y][princess_point->x] = PRINCESS;

    sys_free_mem(hero_point);
    sys_free_mem(dragon_point);
    sys_free_mem(princess_point);
    ll_clear_free(list, true);

    bool found = false;
    coordinate_t finish_line;

    //Find a valid point to put the finish line.
    while(!found)
    {
        bool cardinal_direc = next_rand_bool();
        bool positive_direc = next_rand_bool();

        int coordinate = (int) (cardinal_direc ? next_random_lim(MAZE_LENGTH - 2) : next_random_lim(MAZE_HEIGHT - 2));

        //Get the coordinate and shift it.
        finish_line = (coordinate_t) {
                .x = cardinal_direc ? coordinate + 1 : (positive_direc ? MAZE_LENGTH - 1 : 0),
                .y = cardinal_direc ? (positive_direc ? MAZE_HEIGHT - 1 : 0) : coordinate + 1
        };
        direction_t direc = cardinal_direc ? (positive_direc ? W : S) : (positive_direc ? A : D);

        coordinate_t shifted = shift(finish_line, direc, 1);

        if(board.board_pieces[shifted.y][shifted.x] == EMPTY)
        {
            found = true;
        }
    }

    //Set the finish spot.
    board.board_pieces[finish_line.y][finish_line.x] = FINISH;

    //Do some cleanup.
    ll_clear_free(list, true);
    sys_free_mem(list);
}

/**
 * @brief Prints the current game board.
 */
void print_board()
{
    for (int y = 0; y < MAZE_HEIGHT; ++y)
    {
        //Create a copy of the string and print it.
        char string[MAZE_LENGTH + 1] = {0};
        memcpy(string, board.board_pieces[y], MAZE_LENGTH);

        println(string);
    }
}

/**
 * @brief The first step of board generations. Fills the board with generic walls.
 */
void generate_board()
{
    //Insert barriers for all locations.
    for (int x = 0; x < MAZE_LENGTH; ++x)
    {
        for (int y = 0; y < MAZE_HEIGHT; ++y)
        {
            if (y % 2 == 0)
            {
                if (x % 2 == 0)
                    board.board_pieces[y][x] = FOUR_WAY_WALL;
                else
                    board.board_pieces[y][x] = HORIZONTAL_WALL;
            }
            else
            {
                if (x % 2 == 0)
                    board.board_pieces[y][x] = VERTICAL_WALL;
                else
                    board.board_pieces[y][x] = EMPTY;
            }
        }
    }

    fill_randomly();
}

void start_dragonmaze_game(void)
{
    board = (maze_board_t) {0};
    dragon_alive = true;
    
    generate_board();
    print_board();
}