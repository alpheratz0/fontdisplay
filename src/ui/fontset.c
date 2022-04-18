#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../base/bitmap.h"
#include "../base/font.h"
#include "../util/debug.h"
#include "../util/numdef.h"
#include "label.h"
#include "fontset.h"

static const char numbers[] = "0 1 2 3 4 5 6 7 8 9";
static const char alphabet_lc1[] = "a b c d e f g h i j k l m";
static const char alphabet_lc2[] = "n o p q r s t u v w x y z";
static const char alphabet_uc1[] = "A  B  C  D  E  F  G  H  I  J  K  L  M";
static const char alphabet_uc2[] = "N  O  P  Q  R  S  T  U  V  W  X  Y  Z";
static const char symbols[] = "~ ! @ # $ % ^ & _ \" ` ' | , . ?";
static const char symbols2[] = "- + * / \\ = [ ] ( ) { } < > ;";

extern fontset_style_t
fontset_style_from(u32 text_color) {
	fontset_style_t style;
	style.text_color = text_color;
	return style;
}

extern fontset_t *
fontset_create(font_t *font, fontset_style_t *style, bool numbers, bool alphabet, bool symbols) {
	fontset_t *fontset;

	if ((fontset = malloc(sizeof(fontset_t)))) {
		fontset->font = font;
		fontset->style = style;
		fontset->numbers = numbers;
		fontset->alphabet = alphabet;
		fontset->symbols = symbols;

		return fontset;
	}

	die("error while calling malloc, no memory available");

	return (void *)(0);
}

extern void
fontset_render_onto(fontset_t *fontset, bitmap_t *bmp) {
	u32 current_x;
	u32 current_y;

	current_y = (bmp->height - fontset->font->height * ((fontset->numbers ? 1 : 0) + (fontset->alphabet ? 4 : 0) + (fontset->symbols ? 2 : 0))) / 2;

	if (fontset->numbers) {
		current_x = (bmp->width - strlen(numbers) * fontset->font->width) / 2;
		label_render_onto(bmp, fontset->font, fontset->style->text_color, numbers, current_x, current_y);
		current_y += fontset->font->height;
	}

	if (fontset->alphabet) {
		current_x = (bmp->width - strlen(alphabet_lc1) * fontset->font->width) / 2;
		label_render_onto(bmp, fontset->font, fontset->style->text_color, alphabet_lc1, current_x, current_y);
		current_y += fontset->font->height;

		current_x = (bmp->width - strlen(alphabet_lc2) * fontset->font->width) / 2;
		label_render_onto(bmp, fontset->font, fontset->style->text_color, alphabet_lc2, current_x, current_y);
		current_y += fontset->font->height;

		current_x = (bmp->width - strlen(alphabet_uc1) * fontset->font->width) / 2;
		label_render_onto(bmp, fontset->font, fontset->style->text_color, alphabet_uc1, current_x, current_y);
		current_y += fontset->font->height;


		current_x = (bmp->width - strlen(alphabet_uc2) * fontset->font->width) / 2;
		label_render_onto(bmp, fontset->font, fontset->style->text_color, alphabet_uc2, current_x, current_y);
		current_y += fontset->font->height;
	}

	if (fontset->symbols) {
		current_x = (bmp->width - strlen(symbols) * fontset->font->width) / 2;
		label_render_onto(bmp, fontset->font, fontset->style->text_color, symbols, current_x, current_y);
		current_y += fontset->font->height;

		current_x = (bmp->width - strlen(symbols2) * fontset->font->width) / 2;
		label_render_onto(bmp, fontset->font, fontset->style->text_color, symbols2, current_x, current_y);
		current_y += fontset->font->height;
	}
}

extern void
fontset_free(fontset_t *fontset) {
	free(fontset);
}
