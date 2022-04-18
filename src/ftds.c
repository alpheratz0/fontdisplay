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

#include <stdbool.h>
#include <string.h>

#include "base/font.h"
#include "ui/fontset.h"
#include "util/debug.h"
#include "util/numdef.h"
#include "x11/keys.h"
#include "x11/window.h"

static char *ffamily = "Iosevka";
static window_t *window;

static bool
match_opt(const char *in, const char *sh, const char *lo) {
	return (strcmp(in, sh) == 0) ||
		   (strcmp(in, lo) == 0);
}

static void
key_press_callback(u32 key) {
	switch (key) {
		case KEY_ESCAPE:
		case KEY_Q:
			window_loop_end(window);
			return;
		default:
			return;
	}
}

static void
usage(void) {
	puts("Usage: ftds [ -hv ] FONT_FAMILY");
	puts("Options are:");
	puts("     -h | --help                    display this message and exit");
	puts("     -v | --version                 display the program version");
	exit(0);
}

static void
version(void) {
	puts("ftds version "VERSION);
	exit(0);
}

int
main(int argc, char **argv) {
	/* skip program name */
	--argc; ++argv;

	if (argc > 0) {
		if (match_opt(*argv, "-h", "--help")) usage();
		else if (match_opt(*argv, "-v", "--version")) version();
		else if (strlen(*argv) > 0 && **argv == '-') dief("invalid option %s", *argv);
		else ffamily = *argv;
	}

	font_t *font;
	fontset_t *fontset;
	fontset_style_t style;

	window = window_create("ftds", "ftds");
	font = font_load(ffamily, 40);
	style = fontset_style_from(0xada54e);
	fontset = fontset_create(font, &style, true, true, true);

	fontset_render_onto(fontset, window->bmp);

	window_set_key_press_callback(window, key_press_callback);
	window_loop_start(window);

	font_unload(font);
	fontset_free(fontset);
	window_free(window);

	return 0;
}
