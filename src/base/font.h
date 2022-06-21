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

#ifndef __FONTDISPLAY_BASE_FONT_H__
#define __FONTDISPLAY_BASE_FONT_H__

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../util/numdef.h"

typedef struct font font_t;

struct font {
	FT_Library library;
	FT_Face face;
	u32 size;
	u32 height;
	u32 width;
};

extern font_t *
font_load(const char *family, u32 size);

extern FT_GlyphSlot
font_get_glyph(font_t *font, char c);

extern void
font_unload(font_t *font);

#endif
