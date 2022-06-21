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

static const char numbers[] = "0 1 2 3 4 5 6 7 8 9";
static const char alphabet_lc1[] = "a b c d e f g h i j k l m";
static const char alphabet_lc2[] = "n o p q r s t u v w x y z";
static const char alphabet_uc1[] = "A  B  C  D  E  F  G  H  I  J  K  L  M";
static const char alphabet_uc2[] = "N  O  P  Q  R  S  T  U  V  W  X  Y  Z";
static const char symbols[] = "~ ! @ # $ % ^ & _ \" ` ' | : , . ?";
static const char symbols2[] = "- + * / \\ = [ ] ( ) { } < > ;";

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
	uint32_t x, y;

	y = (
		bmp->height - 
			fontset->style->font->height * (
				(fontset->charset & CHARSET_NUMBERS ? 1 : 0) +
				(fontset->charset & CHARSET_ALPHABET ? 4 : 0) +
				(fontset->charset & CHARSET_SYMBOLS ? 2 : 0))
		) / 2;

	if (fontset->charset & CHARSET_NUMBERS) {
		x = (bmp->width - strlen(numbers) * fontset->style->font->width) / 2;
		label_render_onto(numbers, fontset->style->font, fontset->style->foreground, x, y, bmp);
		y += fontset->style->font->height;
	}

	if (fontset->charset & CHARSET_ALPHABET) {
		x = (bmp->width - strlen(alphabet_lc1) * fontset->style->font->width) / 2;
		label_render_onto(alphabet_lc1, fontset->style->font, fontset->style->foreground, x, y, bmp);
		y += fontset->style->font->height;

		x = (bmp->width - strlen(alphabet_lc2) * fontset->style->font->width) / 2;
		label_render_onto(alphabet_lc2, fontset->style->font, fontset->style->foreground, x, y, bmp);
		y += fontset->style->font->height;

		x = (bmp->width - strlen(alphabet_uc1) * fontset->style->font->width) / 2;
		label_render_onto(alphabet_uc1, fontset->style->font, fontset->style->foreground, x, y, bmp);
		y += fontset->style->font->height;

		x = (bmp->width - strlen(alphabet_uc2) * fontset->style->font->width) / 2;
		label_render_onto(alphabet_uc2, fontset->style->font, fontset->style->foreground, x, y, bmp);
		y += fontset->style->font->height;
	}

	if (fontset->charset & CHARSET_SYMBOLS) {
		x = (bmp->width - strlen(symbols) * fontset->style->font->width) / 2;
		label_render_onto(symbols, fontset->style->font, fontset->style->foreground, x, y, bmp);
		y += fontset->style->font->height;

		x = (bmp->width - strlen(symbols2) * fontset->style->font->width) / 2;
		label_render_onto(symbols2, fontset->style->font, fontset->style->foreground, x, y, bmp);
		y += fontset->style->font->height;
	}
}

extern void
fontset_free(fontset_t *fontset)
{
	free(fontset);
}
