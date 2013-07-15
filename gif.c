#include "gif.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


GIF* GIF_create(Byte w, Byte h)
{
	GIF* gif;
	
	gif = malloc(sizeof(*gif));
	
	gif->w = w;
	gif->h = h;
	gif->global_palette = false;
	gif->color_res = 7;
	gif->sort = false;
	gif->palette_size = 0;
	gif->palette = NULL;
	gif->bg_color = 0;
	gif->px_ratio = 0;
	gif->first = gif->last = NULL;
	
	return gif;
}

GIFColor* GIF_set_palette(GIF* gif, Byte palette_size)
{
	GIFColor* palette;
	
	gif->global_palette = true;
	gif->color_res = palette_size;
	gif->palette_size = palette_size;
	
	palette = calloc(sizeof(*palette), 2<<palette_size);
	gif->palette = palette;
	return palette;
}

void GIFFrame_set_bg(GIF* gif, GIFColorIndex color)
{
	gif->bg_color = color;
}

void GIF_add_frame(GIF* gif, GIFFrame* frame)
{
	if(gif->last)
		gif->last->next = frame;
	else
		gif->first = frame;
	gif->last = frame;
}

void GIF_delete(GIF* gif)
{
	GIFFrame* tmp;
	while((tmp = gif->first)) {
		gif->first = tmp->next;
		free(tmp);
	}
	
	if(gif->palette)
		free(gif->palette);
	
	free(gif);
}


GIFFrame* GIFFrame_create(Byte x, Byte y, Byte w, Byte h)
{
	GIFFrame* frame;
	
	frame = malloc(sizeof(*frame) + w * h * sizeof(*frame->pixels));
	
	frame->disposal = DISPOSE_UNSPECIFIED;
	frame->user_input = false;
	frame->transparence = false;
	frame->delay = 0;
	frame->transparent_color = 0;
	frame->x = x;
	frame->y = y;
	frame->w = w;
	frame->h = h;
	frame->local_palette = false;
	frame->interlace = false;
	frame->sort = false;
	frame->palette_size = 0;
	frame->palette = NULL;
	
	for(size_t i = 0;  i < w*h;  i++)
		frame->pixels[i] = 0;
	
	return frame;
}

GIFColor* GIFFrame_set_palette(GIFFrame* frame, Byte palette_size)
{
	GIFColor* palette;
	
	frame->local_palette = true;
	frame->palette_size = palette_size;
	
	palette = calloc(sizeof(*palette), 2<<palette_size);
	frame->palette = palette;
	return palette;
}

void GIFFrame_set_transparence(GIFFrame* frame, GIFColorIndex color)
{
	frame->transparence = true;
	frame->transparent_color = color;
}

void GIFFrame_delete(GIFFrame* frame)
{
	if(frame->palette)
		free(frame->palette);
	free(frame);
}



static void write_bytes(FILE* out, size_t size, size_t nmemb, const void* p)
{
#ifdef LITTLE_ENDIAN
	fwrite(p, size, nmemb, out);
#else
	while(nmemb--) {
		for(size_t i = size;  i;  i--)
			fwrite((const char*)p+i-1, 1, 1, out);
		(const char*)p += size;
	}
#endif
}

static void write_bitfield(FILE* out, ...)
{
	Byte byte, bits;
	size_t sz;
	va_list args;
	
	va_start(args, out);
	byte = 0;
	
	for(size_t off = 0;  off < 8;  off += sz) {
		sz = va_arg(args, int);
		bits = va_arg(args, int);
		byte <<= sz;
		byte |= bits & ((1<<sz)-1);
	}
	
	va_end(args);
	fwrite(&byte, 1, 1, out);
}

bool GIF_write(const GIF* gif, const char* filename)
{
	FILE* out;
	
	out = fopen(filename, "wb");
	if(!out)
		return false;
	
	/* “Header”. */
	write_bytes(out, 1, 6, "GIF89a");
	/* “Logical Screen Descriptor”. */
	write_bytes(out, 2, 1, &gif->w);
	write_bytes(out, 2, 1, &gif->h);
	write_bitfield(out,
	  1, gif->global_palette,
	  3, gif->color_res,
	  1, gif->sort,
	  3, gif->palette_size);
	write_bytes(out, 1, 1, &gif->bg_color);
	write_bytes(out, 1, 1, &gif->px_ratio);
	
	/* “Global Color Table”. */
	if(gif->global_palette) {
		for(size_t i = 0;  i < (unsigned)2<<gif->palette_size;  i++) {
			write_bytes(out, 1, 1, &gif->palette[i].r);
			write_bytes(out, 1, 1, &gif->palette[i].g);
			write_bytes(out, 1, 1, &gif->palette[i].b);
		}
	}
	
	/* Loop count (Netscape “Application Extension Block”). */
	write_bytes(out, 1, 19, "!\xFF\x0BNETSCAPE2.0\x03\x01\xFF\xFF\x00");
	
	/* Frames. */
	for(GIFFrame* frame = gif->first;  frame;  frame = frame->next) {
		/* “Graphic Control Extension”. */
		write_bytes(out, 1, 3, "!\xF9\x04");
		write_bitfield(out,
		  3, 0x00,
		  3, frame->disposal,
		  1, frame->user_input,
		  1, frame->transparence);
		write_bytes(out, 1, 2, &frame->delay);
		write_bytes(out, 1, 1, &frame->transparent_color);
		write_bytes(out, 1, 1, "\x00");
		/* “Image Descriptor”. */
		write_bytes(out, 1, 1, ",");
		write_bytes(out, 1, 2, &frame->x);
		write_bytes(out, 1, 2, &frame->y);
		write_bytes(out, 1, 2, &frame->w);
		write_bytes(out, 1, 2, &frame->h);
		write_bitfield(out,
		  1, frame->local_palette,
		  1, frame->interlace,
		  1, frame->sort,
		  2, 0x00,
		  3, frame->palette_size);
		/* “Local Color Table”. */
		if(frame->local_palette) {
			for(size_t i = 0;  i < (unsigned)2<<frame->palette_size;  i++) {
				write_bytes(out, 1, 1, &frame->palette[i].r);
				write_bytes(out, 1, 1, &frame->palette[i].g);
				write_bytes(out, 1, 1, &frame->palette[i].b);
			}
		}
		/* “Image Data”. */
		Byte px_size = /*(frame->local_palette) ?
		  frame->palette_size + 1
		  : gif->palette_size + 1*/8;
		size_t encoded_size;
		Byte* buf = malloc(2 * gif->w * gif->h);
		encoded_size = lzw_encode(px_size, frame->w*frame->h, frame->pixels, buf);
		Byte sz;
		write_bytes(out, 1, 1, &px_size);
		for(size_t i = 0;  i < encoded_size;  i += sz) {
			sz = (encoded_size-i >= 255) ? 255 : encoded_size-i;
			write_bytes(out, 1, 1, &sz);
			write_bytes(out, 1, sz, buf+i);
		}
		write_bytes(out, 1, 1, "\x00");
		free(buf);
	}
	
	/* “Trailer”. */
	write_bytes(out, 1, 1, ";");
	
	fclose(out);
	return true;
}
