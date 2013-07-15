#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "typedefs.h"
#include "gif.h"
#include "maze.h"

#define  DEFAULT_WIDTH   16
#define  DEFAULT_HEIGHT  16

/* LZW encoder test program. */
/*int main(void)
{
	Byte in_bytes[256];
	Byte out_bytes[256];
	int c;
	size_t len, compressed_len;
	
	for(len = 0;  (c = getchar()) != EOF;  len++)
		in_bytes[len] = c, fprintf(stderr, "%02X ", in_bytes[len]);
	
	compressed_len = lzw_encode(BYTE_SZ, len, in_bytes, out_bytes);
	fprintf(stderr, "\nlen %u / compressed %u\n", len, compressed_len);
	
	for(size_t i = 0;  i < compressed_len;  i++)
		putchar(out_bytes[i]), fprintf(stderr, "%02X ", out_bytes[i]);
	putc('\n', stderr);
	
	return EXIT_SUCCESS;
}*/

bool uint_of_str(const char* s, size_t* x)
{
	char* p;
	*x = strtoul(s, &p, 0);
	return p != s;
}

/* Maze generator test program. */
int main(int ac, char** av)
{
	/*Maze* maze;
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
	
	maze_delete(maze);
	return EXIT_SUCCESS;*/
	
	GIF* gif;
	GIFColor* palette;
	GIFFrame* frame;
	
	gif = GIF_create(4,4);
	palette = GIF_set_palette(gif, 1);
	palette[0] = (GIFColor){0x22, 0x22, 0x77};
	palette[1] = (GIFColor){0x22, 0x88, 0x22};
	palette[2] = (GIFColor){0x99, 0x33, 0x33};
	palette[3] = (GIFColor){0x44, 0xAA, 0xAA};
	
	frame = GIFFrame_create(0, 0, 3, 3);
	frame->disposal = DISPOSE_NOT;
	frame->delay = 100;
	frame->pixels[0] = 0;
	frame->pixels[1] = 1;
	frame->pixels[2] = 3;
	frame->pixels[3] = 3;
	frame->pixels[4] = 0;
	frame->pixels[5] = 1;
	frame->pixels[6] = 1;
	frame->pixels[7] = 3;
	frame->pixels[8] = 0;
	GIF_add_frame(gif, frame);
	
	frame = GIFFrame_create(2, 2, 2, 2);
	frame->disposal = DISPOSE_BG;
	frame->user_input = true;
	frame->delay = 100;
	for(size_t i = 0;  i < frame->w*frame->h;  i++)
		frame->pixels[i] = 2;
	GIF_add_frame(gif, frame);
	
	if(!GIF_write(gif, "out.gif"))
		perror("out.gif");
	
	GIF_delete(gif);
	return EXIT_SUCCESS;
}
