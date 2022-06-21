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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../base/bitmap.h"
#include "../base/font.h"
#include "../util/debug.h"
#include "label.h"
#include "fontset.h"

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

static const char *numbers[] = {
	"0 1 2 3 4 5 6 7 8 9"
};

static const char *alphabet[] = {
	"a b c d e f g h i j k l m",
	"n o p q r s t u v w x y z",
	"A  B  C  D  E  F  G  H  I  J  K  L  M",
	"N  O  P  Q  R  S  T  U  V  W  X  Y  Z"
};

static const char *symbols[] = {
	"~ ! @ # $ % ^ & _ \" ` ' | : , . ?",
	"- + * / \\ = [ ] ( ) { } < > ;"
};


extern fontset_style_t
fontset_style_from(font_t *font, uint32_t foreground)
{
	fontset_style_t style;

	style.font = font;
	style.foreground = foreground;

	return style;
}

extern fontset_t *
fontset_create(fontset_style_t *style, enum charset charset)
{
	fontset_t *fontset;

	if (NULL == (fontset = malloc(sizeof(fontset_t)))) {
		die("error while calling malloc, no memory available");
	}

	fontset->style = style;
	fontset->charset = charset;

	return fontset;
}

extern void
fontset_render_onto(fontset_t *fontset, bitmap_t *bmp)
{
	uint32_t y;

	y = (
		bmp->height - 
			fontset->style->font->height * (
				(fontset->charset & CHARSET_NUMBERS ? 1 : 0) +
				(fontset->charset & CHARSET_ALPHABET ? 4 : 0) +
				(fontset->charset & CHARSET_SYMBOLS ? 2 : 0))
		) / 2;

#define CHARSET_RENDER(ch) do {                                 \
	size_t i;                                                   \
	font_t *font;                                               \
	font = fontset->style->font;                                \
	for (i = 0; i < ARRAY_LENGTH(ch); i++) {                    \
		label_render_onto(                                      \
			ch[i], font, fontset->style->foreground,            \
			(bmp->width - strlen(ch[i]) * font->width) / 2,     \
			y, bmp                                              \
		);                                                      \
		y += font->height;                                      \
	}                                                           \
} while (0)

	if (fontset->charset & CHARSET_NUMBERS) {
		CHARSET_RENDER(numbers);
	}

	if (fontset->charset & CHARSET_ALPHABET) {
		CHARSET_RENDER(alphabet);
	}

	if (fontset->charset & CHARSET_SYMBOLS) {
		CHARSET_RENDER(symbols);
	}
}

extern void
fontset_free(fontset_t *fontset)
{
	free(fontset);
}
