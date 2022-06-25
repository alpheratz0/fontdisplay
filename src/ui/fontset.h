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

#include "../base/bitmap.h"
#include "../base/font.h"

enum charset {
	CHARSET_ALPHABET = 1,
	CHARSET_NUMBERS  = 1 << 1,
	CHARSET_SYMBOLS  = 1 << 2,
	CHARSET_ALL = CHARSET_ALPHABET |
	              CHARSET_SYMBOLS |
	              CHARSET_NUMBERS
};

struct fontset_style {
	struct font *font;
	uint32_t foreground;
};

struct fontset {
	struct fontset_style *style;
	enum charset charset;
};

extern struct fontset_style
fontset_style_from(struct font *font, uint32_t foreground);

extern struct fontset *
fontset_create(struct fontset_style *style, enum charset charset);

extern void
fontset_render_onto(struct fontset *fontset, struct bitmap *bmp);

extern void
fontset_free(struct fontset *fontset);

#endif
