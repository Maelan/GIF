#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "typedefs.h"
#include "gif.h"
#include "maze.h"

#define  DEFAULT_WIDTH   16
#define  DEFAULT_HEIGHT  16

bool uint_of_str(const char* s, size_t* x)
{
	char* p;
	*x = strtoul(s, &p, 0);
	return p != s;
}

int main(int ac, char** av)
{
	/* Maze generation. */
	
	Maze* maze;
	size_t w, h;
	
	srand(time(NULL));
	
	w = DEFAULT_WIDTH;
	h = DEFAULT_HEIGHT;
	if( (ac >= 2 && !uint_of_str(av[1], &w))
	 || (ac >= 3 && !uint_of_str(av[2], &h)) ) {
		fputs("Invalid number.\n", stderr);
		exit(EXIT_FAILURE);
	}
	
	maze = maze_generate(w, h);
	
	maze_print(maze, false);
	maze_print_skeletton(maze, false);
	
	
	/* GIF writing. */
	
	GIF* gif;
	GIFColor* palette;
	GIFFrame* frame;
	
	gif = GIF_create(8*w+1, 8*h+1);
	
	palette = GIF_set_palette(gif, 1);
	/*palette[0] = GIFCOLOR(0x222277);
	palette[1] = GIFCOLOR(0x228822);
	palette[2] = GIFCOLOR(0x993333);
	palette[3] = GIFCOLOR(0x44AAAA);*/
	palette[0] = GIFCOLOR(rand() % 0x1000000);
	palette[1] = GIFCOLOR(rand() % 0x1000000);
	palette[2] = GIFCOLOR(rand() % 0x1000000);
	palette[3] = GIFCOLOR(rand() % 0x1000000);
	
	gif->loop_count = LOOP_FOREVER;
	
	size_t i, j;
	Direction dir;
	queue_rewind(&maze->moves);
	
	while(!queue_is_empty(&maze->moves)) {
		queue_pop(&i, &j, &dir, &maze->moves);
		if(dir%2)
			frame = GIFFrame_create(0, 0, 7, 8);
		else
			frame = GIFFrame_create(0, 0, 8, 7);
		frame->disposal = DISPOSE_NOT;
		frame->delay = 3;
		GIFColorIndex color = rand() % 4;
		for(size_t i = 0;  i < (unsigned)frame->w*frame->h;  i++)
			frame->pixels[i] = /*2*/color;
		frame->x = 8*j+1;
		frame->y = 8*i+1;
		if(dir == RIGHT)
			frame->x--;
		else if(dir == BOTTOM)
			frame->y--;
		
		GIF_add_frame(gif, frame);
	}
	
	/* Pause before looping (empty frame). */
	frame = GIFFrame_create(0, 0, 1, 1);
	frame->delay = 1500;
	GIFFrame_set_transparence(frame, 0);
	GIF_add_frame(gif, frame);
	
	if(!GIF_write(gif, "out.gif"))
		perror("out.gif");
	
	GIF_delete(gif);
	maze_delete(maze);
	return EXIT_SUCCESS;
}
