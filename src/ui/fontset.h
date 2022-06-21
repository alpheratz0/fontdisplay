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
