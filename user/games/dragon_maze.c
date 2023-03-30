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
#include "print_format.h"

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

///An enum representing the difficulty of the running game.
typedef enum {
    ///In easy mode, there is no added challenge.
    EASY,
    ///In normal mode, the maze is initially obscured, but can be mapped out.
    NORMAL,
    ///In hard mode, the dragon is smarter and can track you down.
    HARD,
} difficulty_t;

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
 * Gets a direction enum from the given character.
 *
 * @param c the character being read.
 * @return the direction.
 */
direction_t direction_from_char(char c)
{
    switch (c)
    {
        case 'W':
        case 'w':
            return W;
        case 'A':
        case 'a':
            return A;
        case 'D':
        case 'd':
            return D;
        case 'S':
        case 's':
            return S;
        default:
            return -1;
    }
}

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

    ///The location of the hero.
    coordinate_t hero_location;
    ///The dragon's location.
    coordinate_t dragon_location;
    ///The princess' location.
    coordinate_t princess_location;
} maze_board_t;

///If the game is currently running.
static bool running = false;
///Denotes if the dragon is still alive.
static bool dragon_alive = false;
///If the hero is currently holding the princess.
static bool holding_princess = false;
///The current hero symbol to use.
static char hero_symbol = HERO_NO_PRINCESS;
///The current difficulty in use.
static difficulty_t difficulty = EASY;
///The active maze board for the current run of the game.
static maze_board_t board;
///A list used to 'inform' the player of something happening.
static linked_list *inform_list;

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
void fill_randomly(void)
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

    //Set the points for all the characters.
    board.hero_location = *hero_point;
    board.dragon_location = *dragon_point;
    board.princess_location = *princess_point;

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
void print_board(void)
{
    clearscr();
    for (int y = 0; y < MAZE_HEIGHT; ++y)
    {
        //Create a copy of the string and print it.
        char string[MAZE_LENGTH + 1] = {0};
        memcpy(string, board.board_pieces[y], MAZE_LENGTH);

        println(string);
    }

    //Print all items from the inform list.
    while(inform_list->_size > 0)
    {
        char *item = remove_item_unsafe(inform_list, 0);
        println(item);
    }
}

/**
 * @brief The first step of board generations. Fills the board with generic walls.
 */
void generate_board(void)
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

/**
 * @brief This function, called once per tick loop, controls hero movement.
 */
void move_hero(void)
{
    if(holding_princess)
    {
        println("Princess: Held\n");
    }
    else
    {
        printf("Princess: %d, %d\n", board.princess_location.x, board.princess_location.y);
    }

    if(!dragon_alive)
    {
        println("Dragon: Defeated\n");
    }
    else
    {
        printf("Dragon: %d, %d\n", board.dragon_location.x, board.dragon_location.y);
    }

    println("Please enter a direction to move. (W, A, S, D) (Press 'F' to do nothing)");

    char next_char = getc();

    int direc = -1;
    while((direc = direction_from_char(next_char)) == -1)
    {
        if(next_char == 'F' || next_char == 'f')
            return;

        println("Please enter a valid direction! (W, A, S, D, F)");
        next_char = getc();
    }

    //Check the coordinate's character.
    direction_t direction = (direction_t) direc;
    coordinate_t shifted = shift(board.hero_location, direction, 1);
    char moving_to = board.board_pieces[shifted.y][shifted.x];

    if(moving_to == FOUR_WAY_WALL || moving_to == VERTICAL_WALL || moving_to == HORIZONTAL_WALL)
    {
        add_item(inform_list, "D'oh!");
        return;
    }

    //Check if the player is moving to the finish.
    if(moving_to == FINISH)
    {
        if(!dragon_alive && !holding_princess)
        {
            add_item(inform_list, "I've already killed the dragon, I need to save the princess!");
            return;
        }

        board.board_pieces[board.hero_location.y][board.hero_location.x] = ' ';
        board.hero_location = shifted;
        board.board_pieces[board.hero_location.y][board.hero_location.x] = hero_symbol;

        add_item(inform_list, "You won!");
        running = false;
        return;
    }

    if(moving_to == PRINCESS)
    {
        holding_princess = true;
        hero_symbol = HERO_WITH_PRINCESS;
    }

    //Update the hero's location.
    board.board_pieces[board.hero_location.y][board.hero_location.x] = ' ';
    board.hero_location = shifted;
    board.board_pieces[board.hero_location.y][board.hero_location.x] = hero_symbol;
}

direction_t find_dragon_movement(void)
{
    if(difficulty == EASY)
        return (direction_t) next_random_lim(4);

    //TODO Implement smarter dragon movement.
    return (direction_t) next_random_lim(4);
}

/**
 * @brief Controls dragon movement, called once per tick loop.
 */
void move_dragon(void)
{

}

void start_dragonmaze_game(void)
{
    //Initialize all the values.
    board = (maze_board_t) {0};
    dragon_alive = true;
    holding_princess = false;
    hero_symbol = HERO_NO_PRINCESS;
    inform_list = nl_unbounded();

    //Ask the user for a difficulty.
    int diff_int = -1;
    while(diff_int == -1)
    {
        println("Which difficulty would you like to play? Easy, Normal, or Hard?");
        char input[11] = {0};
        gets(input, 10);

        //Check if the string is valid for any difficulty.
        if(strcicmp(input, "easy") == 0)
            diff_int = EASY;
        else if(strcicmp(input, "normal") == 0)
            diff_int = NORMAL;
        else if(strcicmp(input, "hard") == 0)
            diff_int = HARD;
    }

    difficulty = (difficulty_t) diff_int;

    generate_board();
    print_board();

    //Begin the game loop.
    running = true;
    while(running)
    {
        move_hero();

        if(dragon_alive)
            move_dragon();

        print_board();
    }

    //Do a final cleanup.
    ll_clear(inform_list);
    sys_free_mem(inform_list);
}