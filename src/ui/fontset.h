#ifndef __FONTDISPLAY_UI_FONTSET_H__
#define __FONTDISPLAY_UI_FONTSET_H__

#include <stdbool.h>

#include "../base/bitmap.h"
#include "../base/font.h"
#include "../util/numdef.h"

typedef struct fontset_style fontset_style_t;
typedef struct fontset fontset_t;

struct fontset_style {
	u32 text_color;
};

struct fontset {
	font_t *font;
	fontset_style_t *style;
	bool numbers;
	bool alphabet;
	bool symbols;
};

extern fontset_style_t
fontset_style_from(u32 text_color);

extern fontset_t *
fontset_create(font_t *font, fontset_style_t *style, bool numbers, bool alphabet, bool symbols);

extern void
fontset_render_onto(fontset_t *fontset, bitmap_t *bmp);

extern void
fontset_free(fontset_t *fontset);

#endif
