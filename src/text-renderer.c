/*
	Copyright (C) 2022-2023 <alpheratz99@protonmail.com>

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License version 2 as published by
	the Free Software Foundation.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#include <pixman.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcft/fcft.h>
#include <string.h>
#include <ctype.h>

#include "log.h"
#include "text-renderer.h"
#include "pixbuf.h"
#include "utils.h"
#include "pen.h"

struct TextRenderer {
	struct fcft_font *font;
};

extern TextRenderer_t *
text_renderer_new(const char *font_family, int size)
{
	TextRenderer_t *tr;
	char font_query[128];

	snprintf(font_query, sizeof(font_query), "%s:size=%d",
			font_family, size);

	fcft_init(FCFT_LOG_COLORIZE_ALWAYS, false, FCFT_LOG_CLASS_NONE);

	if (!fcft_set_scaling_filter(FCFT_SCALING_FILTER_LANCZOS3))
		die("fcft_set_scaling_filter failed");

	tr = xcalloc(1, sizeof(TextRenderer_t));

	tr->font = fcft_from_name(1, (const char *[]){font_query}, NULL);

	if (NULL == tr->font) {
		die("fcft_from_name couldn't load font: %s:size=%d",
				font_family, size);
	}

	fcft_set_emoji_presentation(tr->font,
			FCFT_EMOJI_PRESENTATION_DEFAULT);

	return tr;
}

extern int
text_renderer_draw_char(TextRenderer_t *tr, Pixbuf_t *pb, Pen_t *pen, char c,
		int x, int y)
{
	const struct fcft_glyph *g;

	g = fcft_rasterize_char_utf32(tr->font, c, FCFT_SUBPIXEL_DEFAULT);

	if (NULL == g ||
			pixman_image_get_format(g->pix) == PIXMAN_a8r8g8b8)
		return 0;

	pixman_image_composite32(PIXMAN_OP_OVER, pen->img, g->pix,
			pixbuf_get_pixman_image(pb), 0, 0, 0, 0, x + g->x,
			y + tr->font->ascent - g->y, g->width, g->height);

	return g->advance.x;
}

extern int
text_renderer_draw_string(TextRenderer_t *tr, Pixbuf_t *pb, Pen_t *pen,
		const char *str, int x, int y)
{
	size_t i, len;
	int w;

	w = 0;
	len = strlen(str);

	for (i = 0; i < len && str[i] != '\n'; ++i) {
		w += text_renderer_draw_char(tr, pb, pen, str[i],
				x+w, y);
	}

	return w;
}

extern int
text_renderer_char_width(const TextRenderer_t *tr, char c)
{
	const struct fcft_glyph *glyph;

	glyph = fcft_rasterize_char_utf32(tr->font, c, FCFT_SUBPIXEL_DEFAULT);

	if (!glyph || pixman_image_get_format(glyph->pix) == PIXMAN_a8r8g8b8)
		return 0;

	return glyph->advance.x;
}

extern int
text_renderer_text_width(const TextRenderer_t *tr, const char *str)
{
	int width;
	for (width = 0; *str; ++str)
		width += text_renderer_char_width(tr, *str);
	return width;
}

extern int
text_renderer_text_height(const TextRenderer_t *tr)
{
	return tr->font->height;
}

extern void
text_renderer_free(TextRenderer_t *tr)
{
	fcft_fini();
	free(tr);
}
