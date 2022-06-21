/*
	Copyright (C) 2022 <alpheratz99@protonmail.com>

	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License version 2 as published by the
	Free Software Foundation.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place, Suite 330, Boston, MA 02111-1307 USA

*/

#ifndef __FONTDISPLAY_UI_FONTSET_H__
#define __FONTDISPLAY_UI_FONTSET_H__

#include <stdint.h>
#include <stdbool.h>

#include "../base/bitmap.h"
#include "../base/font.h"

typedef struct fontset_style fontset_style_t;
typedef struct fontset fontset_t;

enum charset {
	CHARSET_ALPHABET = 1,
	CHARSET_NUMBERS  = 1 << 1,
	CHARSET_SYMBOLS  = 1 << 2,
	CHARSET_ALL = CHARSET_ALPHABET |
	              CHARSET_SYMBOLS |
	              CHARSET_NUMBERS
};

struct fontset_style {
	font_t *font;
	uint32_t foreground;
};

struct fontset {
	fontset_style_t *style;
	enum charset charset;
};

extern fontset_style_t
fontset_style_from(font_t *font, uint32_t foreground);

extern fontset_t *
fontset_create(fontset_style_t *style, enum charset charset);

extern void
fontset_render_onto(fontset_t *fontset, bitmap_t *bmp);

extern void
fontset_free(fontset_t *fontset);

#endif
