#ifndef INCLUDED_GIF
#define INCLUDED_GIF

#include "typedefs.h"
#include "lzw_encoder.h"


typedef enum {
	DISPOSE_UNSPECIFIED,
	DISPOSE_NOT,
	DISPOSE_BG,
	DISPOSE_PREV
} GIFDisposalMethod;

#define  LOOP_FOREVER  0xFFFF

typedef struct {
	Byte r, g, b;
} GIFColor;

typedef Byte2 GIFUnsigned;
typedef Byte GIFBitField;
typedef Byte GIFColorIndex;

typedef struct GIFFrame {
	struct GIFFrame* next;
	/* “Graphic Control Extension”. */
	Byte reserved2    :3,
	     disposal     :3,
	     user_input   :1,
	     transparence :1;
	Byte2 delay;
	Byte transparent_color;
	/* “Image Descriptor”. */
	Byte2 x, y;
	Byte2 w, h;
	Byte local_palette :1,
	     interlace     :1,
	     sort          :1,
	     reserved      :2,
	     palette_size  :3;
	/* “Local Color Table“. */
	GIFColor* palette;
	/* Raw (unencoded) data. */
	Byte pixels[];
} GIFFrame;

typedef struct GIF {
	/* “Logical Screen Descriptor”. */
	Byte2 w, h;
	Byte global_palette :1,
	     color_res      :3,
	     sort           :1,
	     palette_size   :3;
	Byte bg_color;
	Byte2 px_ratio;
	/* “Global Color Table“. */
	GIFColor* palette;
	/* Loop count (Netscape “Application Extension Block”). */
	Byte2 loop_count;
	/* Frames. */
	GIFFrame *first, *last;
} GIF;


GIF* GIF_create(Byte w, Byte h);

GIFColor* GIF_set_palette(GIF* gif, Byte palette_size);

void GIFFrame_set_bg(GIF* gif, GIFColorIndex color);

void GIF_add_frame(GIF* gif, GIFFrame* frame);

void GIF_delete(GIF* gif);


GIFFrame* GIFFrame_create(Byte x, Byte y, Byte w, Byte h);

GIFColor* GIFFrame_set_palette(GIFFrame* frame, Byte palette_size);

void GIFFrame_set_transparence(GIFFrame* frame, GIFColorIndex color);

void GIFFrame_delete(GIFFrame* frame);


GIFColor GIFCOLOR(unsigned code);


bool GIF_write(const GIF* gif, const char* filename);


#endif
