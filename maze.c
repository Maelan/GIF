#include "maze.h"
#include <stdlib.h>
#include <stdio.h>

static Maze* maze_create(size_t w, size_t h)
{
	Maze* maze;
	
	maze = malloc(sizeof(*maze));
	maze->w = w;
	maze->h = h;
	maze->unvisited_count = w*h;
	
	maze->cells = malloc(h * sizeof(*maze->cells));
	for(size_t i = 0;  i < h;  i++)
		maze->cells[i] = calloc(w, sizeof(**maze->cells));
	
	return maze;
}

void maze_delete(Maze* maze)
{
	for(size_t i = 0;  i < maze->h;  i++)
		free(maze->cells[i]);
	free(maze->cells);
	free(maze);
}

typedef struct {
	size_t i, j;
} Position;

typedef struct QueueNode {
	Position pos;
	struct QueueNode* next;
} QueueNode;

typedef struct {
	QueueNode *head, *tail;
} Queue;

#define QUEUE_INIT  ( (Queue){NULL,NULL} )

static bool queue_is_empty(const Queue* queue)
{
	return !queue->head;
}

static void queue_append(Position pos, Queue* queue)
{
	QueueNode* node = malloc(sizeof(*node));
	node->pos = pos;
	node->next = NULL;
	
	if(queue->tail)
		queue->tail->next = node;
	else
		queue->head = node;
	queue->tail = node;
}

static Position queue_pop(Queue* queue)
{
	QueueNode* tmp = queue->head;
	Position pos = tmp->pos;
	queue->head = tmp->next;
	if(!queue->head)
		queue->tail = NULL;
	free(tmp);
	return pos;
}

static unsigned random(unsigned max)
{
	return rand() % max;
}

static Direction choose_direction(unsigned mask)
{
	Direction dir;
	do
		dir = random(4);
	while(!(mask & DIR_MASK(dir)));
	return dir;
}

static Direction opposite_direction(Direction dir)
{
	return (dir+2) % 4;
}

unsigned neighbourhood(const Maze* maze, size_t i, size_t j, bool visited, size_t* count)
{
	unsigned mask = 0;
	*count = 0;
	
	if(j > 0 && (bool)maze->cells[i][j-1].visited == visited) {
		mask |= LEFTm;
		(*count)++;
	}
	if(i > 0 && (bool)maze->cells[i-1][j].visited == visited) {
		mask |= TOPm;
		(*count)++;
	}
	if(j < maze->w-1 && (bool)maze->cells[i][j+1].visited == visited) {
		mask |= RIGHTm;
		(*count)++;
	}
	if(i < maze->h-1 && (bool)maze->cells[i+1][j].visited == visited) {
		mask |= BOTTOMm;
		(*count)++;
	}
	
	return mask;
}

static void move_ij(size_t* i, size_t* j, Direction dir)
{
	switch(dir) {
	  case LEFT:      (*j)--;  break;
	  case RIGHT:     (*j)++;  break;
	  case TOP:       (*i)--;  break;
	  case BOTTOM:    (*i)++;  break;
	}
}

static void move_here
  (Maze* maze, Queue* cells,
   size_t i, size_t j, Direction dir)
{
	if(i < maze->h && j < maze->w)
		maze->cells[i][j].ways |= DIR_MASK(dir);
	move_ij(&i, &j, dir);
	maze->cells[i][j].ways |= DIR_MASK(opposite_direction(dir));
	maze->cells[i][j].visited = true;
	maze->unvisited_count--;
	
	queue_append((Position){i,j}, cells);
}

static void search_unvisited
  (Maze* maze, Queue* cells)
{
	size_t count;
	unsigned mask; 
	Direction dir;
	
	for(size_t i = 0;  i < maze->h;  i++) {
		for(size_t j = 0;  j < maze->w;  j++) {
			if( maze->cells[i][j].visited
			 || !(mask = neighbourhood(maze, i, j, true, &count)) )
				continue;
			dir = choose_direction(mask);
			move_ij(&i, &j, dir);
			move_here(maze, cells, i, j, opposite_direction(dir));
			return;
		}
	}
}

Maze* maze_generate(size_t w, size_t h)
{
	Maze* maze;
	Queue cells;
	Position pos;
	Direction dir;
	size_t i, j;
	
	unsigned mask = 0;
	size_t count = 0;
	size_t branching = 0;
	unsigned random_branching;
	
	maze = maze_create(w, h);
	cells = QUEUE_INIT;
	
	move_here(maze, &cells, 0, -1, RIGHT);
	//move_here(maze, &cells, -1, w-1, BOTTOM);
	move_here(maze, &cells, h-1, w, LEFT);
	//move_here(maze, &cells, h, 0, TOP);
	
	while(maze->unvisited_count) {
		if(queue_is_empty(&cells))
			search_unvisited(maze, &cells);
		pos = queue_pop(&cells);
		i = pos.i;
		j = pos.j;
		mask = neighbourhood(maze, i, j, false, &count);
		if(count) {
			branching = 1;
			random_branching = random(1000);
			if(count >= 2 && random_branching < 16) {
				branching++;
				if(count >= 3 && random_branching < 4)
					branching++;
			}
			while(branching--) {
				dir = choose_direction(mask);
				mask ^= DIR_MASK(dir);
				move_here(maze, &cells, i, j, dir);
			}
		}
	}
	
	return maze;
}

static const char* symbols[] =
  { " ", "╴", "╵", "┘", "╶", "─", "└", "┴",
    "╷", "┐", "│", "┤", "┌", "┬", "├", "┼"  };

void maze_print(const Maze* maze, bool print_header)
{
	size_t w = maze->w,  h = maze->h;
	bool left, rght, top, botm;
	
	if(print_header)
		printf("%u×%u:\n", maze->w, maze->h);
	
	for(size_t i = 0;  i <= h;  i++) {
		for(size_t j = 0;  j <= w;  j++) {
			left = j > 0 && (i == 0 || !(maze->cells[i-1][j-1].ways & BOTTOMm));
			top =  i > 0 && (j == 0 || !(maze->cells[i-1][j-1].ways & RIGHTm));
			rght = j < w && (i == 0 || !(maze->cells[i-1][j].ways & BOTTOMm));
			botm = i < h && (j == 0 || !(maze->cells[i][j-1].ways & RIGHTm));
			if(left)
				fputs("─", stdout);
			else if(j)
				putchar(' ');
			fputs(symbols[left + 2*top + 4*rght + 8*botm], stdout);
		}
		putchar('\n');
	}
}

void maze_print_skeletton(const Maze* maze, bool print_header)
{
	
	size_t w = maze->w,  h = maze->h;
	
	if(print_header)
		printf("%u×%u:\n", maze->w, maze->h);
	
	for(size_t i = 0;  i < h;  i++) {
		for(size_t j = 0;  j < w;  j++) {
			if(j && maze->cells[i][j].ways & LEFTm)
				fputs("─", stdout);
			else
				putchar(' ');
			fputs(symbols[maze->cells[i][j].ways], stdout);
		}
		putchar('\n');
	}
}
