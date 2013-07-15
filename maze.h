#ifndef INCLUDED_MAZE
#define INCLUDE_MAZE

#include <stddef.h>
#include <stdbool.h>

typedef enum {LEFT, TOP, RIGHT, BOTTOM} Direction;

#define  DIR_MASK(dir)  ( 1<<(dir) )

typedef enum {
	LEFTm =   DIR_MASK(LEFT),
	TOPm =    DIR_MASK(TOP),
	RIGHTm =  DIR_MASK(RIGHT),
	BOTTOMm = DIR_MASK(BOTTOM)
} DirectionMask;

typedef struct {
	unsigned ways    :4,
	         visited :1;
} Cell;

typedef struct {
	size_t w, h;
	size_t unvisited_count;
	Cell** cells;
} Maze;

Maze* maze_generate(size_t w, size_t h);

void maze_delete(Maze* maze);

void maze_print(const Maze* maze, bool print_header);

void maze_print_skeletton(const Maze* maze, bool print_header);

#endif
