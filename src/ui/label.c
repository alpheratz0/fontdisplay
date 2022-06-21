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

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../base/bitmap.h"
#include "../base/font.h"
#include "../util/debug.h"
#include "../util/color.h"
#include "../util/numdef.h"
#include "label.h"

static void
label_render_char_onto(bitmap_t *bmp, font_t *font, u32 color, char c, u32 x, u32 y) {
	FT_GlyphSlot glyph = font_get_glyph(font, c);
	u32 glyph_h = glyph->bitmap.rows;
	u32 glyph_w = glyph->bitmap.width;

	for (u32 i = 0; i < glyph_h; ++i) {
		for (u32 j = 0; j < glyph_w; ++j) {
			u32 xmap = x + j + glyph->bitmap_left;
			u32 ymap = y + i - glyph->bitmap_top + font->size;
			u8 gray = glyph->bitmap.buffer[i*glyph_w+j];

			bitmap_set(bmp, xmap, ymap, color_lerp(bitmap_get(bmp, xmap, ymap), color, gray));
		}
	}
}

extern void
label_render_onto(bitmap_t *bmp, font_t *font, u32 color, const char *text, u32 x, u32 y) {
	for (size_t i = 0; i < strlen(text) && text[i] != '\n'; ++i) {
		label_render_char_onto(bmp, font, color, text[i], x + i * font->width, y);
	}
}
