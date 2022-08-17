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

	 ____________________
	( fonts are awesome! )
	 --------------------
	   o
	    o
	     o
	        __  /  __
	       /  \ | /  \
	           \|/
	       _.---v---.,_
	      /            \  /\__/\
	     /              \ \_  _/
	     |__ @           |_/ /
	      _/                /
	      \       \__,     /
	   ~~~~\~~~~~~~~~~~~~~`~~~

*/

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fontconfig/fontconfig.h>
#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xproto.h>

#define UNUSED                             __attribute__((unused))

#define TEXT_COLOR                         (0xada54e)

#define CHKFTERR(name,err) do {                                 \
	FT_Error error;                                             \
	error = (err);                                              \
	if (error != 0) {                                           \
		dief(name " failed with error code: %d", (int)(error)); \
	}                                                           \
} while (0)

/* X11 */
static xcb_connection_t *conn;
static xcb_window_t window;
static xcb_gcontext_t gc;
static xcb_image_t *image;
static uint16_t swidth, sheight;
static uint32_t *px;

/* FT */
static FT_Library ftlib;
static FT_Face ftface;
static uint32_t ftsize;
static uint32_t ftwidth, ftheight;

static const char *text[] = {
	"0 1 2 3 4 5 6 7 8 9",
	"a b c d e f g h i j k l m",
	"n o p q r s t u v w x y z",
	"A  B  C  D  E  F  G  H  I  J  K  L  M",
	"N  O  P  Q  R  S  T  U  V  W  X  Y  Z",
	"~ ! @ # $ % ^ & _ \" ` ' | : , . ?",
	"- + * / \\ = [ ] ( ) { } < > ;"
};


static void
die(const char *err)
{
	fprintf(stderr, "fontdisplay: %s\n", err);
	exit(1);
}

static void
dief(const char *fmt, ...)
{
	va_list args;

	fputs("fontdisplay: ", stderr);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n', stderr);
	exit(1);
}

static void *
xmalloc(size_t size)
{
	void *p;

	if (NULL == (p = malloc(size))) {
		die("error while calling malloc, no memory available");
	}

	return p;
}

static uint32_t
color_lerp(uint32_t from, uint32_t to, uint8_t v)
{
	uint8_t r, g, b;

#define BLERP(f,t,s) ((f)+(((t)-(f))*(s))/0xff)

	r = BLERP((from >> 16) & 0xff, (to >> 16) & 0xff, v);
	g = BLERP((from >> 8) & 0xff, (to >> 8) & 0xff, v);
	b = BLERP(from & 0xff, to & 0xff, v);

#undef BLERP

	return (r << 16) | (g << 8) | b;
}

static void
usage(void)
{
	puts("usage: fontdisplay [-hv] [font_family]");
	exit(0);
}

static void
version(void)
{
	puts("fontdisplay version "VERSION);
	exit(0);
}

static char *
search_font(const char *family)
{
	FcPattern *pattern;
	FcPattern *match;
	FcResult result;
	FcValue v;
	char *path = NULL;

	if ((pattern = FcNameParse((const FcChar8 *)(family)))) {
		FcConfigSubstitute(0, pattern, FcMatchPattern);
		FcDefaultSubstitute(pattern);

		if ((match = FcFontMatch(0, pattern, &result))) {
			FcPatternGet(match, FC_FAMILY, 0, &v);

			if (strcmp(family, (char *)(v.u.s)) == 0) {
				FcPatternGet(match, FC_FILE, 0, &v);
				path = strdup((char *)(v.u.s));
			}

			FcPatternDestroy(match);
		}

		FcPatternDestroy(pattern);
		FcFini();
	}

	return path;
}

static void
load_font(const char *family, uint32_t size)
{
	char *path;

	if (NULL == (path = search_font(family))) {
		dief("font family not found: %s", family);
	}

	CHKFTERR("FT_Init_FreeType", FT_Init_FreeType(&ftlib));
	CHKFTERR("FT_New_Face", FT_New_Face(ftlib, path, 0, &ftface));
	CHKFTERR("FT_Set_Char_Size", FT_Set_Char_Size(ftface, 0, size * 64, 72, 72));
	CHKFTERR("FT_Load_Char", FT_Load_Char(ftface, '0', FT_LOAD_RENDER));

	ftsize = size;
	ftwidth = ftface->glyph->advance.x >> 6;
	ftheight = (ftface->size->metrics.ascender - ftface->size->metrics.descender) >> 6;

	free(path);
}

static void
unload_font(void)
{
	FT_Done_FreeType(ftlib);
}

static void
render_char(char c, uint32_t x, uint32_t y)
{
	FT_GlyphSlot glyph;
	uint32_t width, height;
	uint32_t xmap, ymap;
	uint32_t gray;
	uint32_t i, j;

	CHKFTERR("FT_Load_Char", FT_Load_Char(ftface, c, FT_LOAD_RENDER));

	glyph = ftface->glyph;
	height = glyph->bitmap.rows;
	width = glyph->bitmap.width;

	for (i = 0; i < height; ++i) {
		for (j = 0; j < width; ++j) {
			xmap = x + j + glyph->bitmap_left;
			ymap = y + i - glyph->bitmap_top + ftsize;
			gray = glyph->bitmap.buffer[i*width+j];

			px[ymap*swidth+xmap] = color_lerp(0, TEXT_COLOR, gray);
		}
	}
}

static void
render_string_centered(const char *str, uint32_t y)
{
	size_t i;
	size_t len;
	uint32_t x;

	len = strlen(str);
	x = (swidth - len * ftwidth) / 2;

	for (i = 0; i < len && str[i] != '\n'; ++i) {
		render_char(str[i], x + i * ftwidth, y);
	}
}

static xcb_atom_t
get_atom(const char *name)
{
	xcb_atom_t atom;
	xcb_generic_error_t *error;
	xcb_intern_atom_cookie_t cookie;
	xcb_intern_atom_reply_t *reply;

	error = NULL;
	cookie = xcb_intern_atom(conn, 0, strlen(name), name);
	reply = xcb_intern_atom_reply(conn, cookie, &error);

	if (NULL != error) {
		dief("xcb_intern_atom failed with error code: %d",
				(int)(error->error_code));
	}

	atom = reply->atom;
	free(reply);

	return atom;
}

static void
create_window(void)
{
	xcb_screen_t *screen;
	uint32_t evmask;

	if (xcb_connection_has_error(conn = xcb_connect(NULL, NULL))) {
		die("can't open display");
	}

	if (NULL == (screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data)) {
		xcb_disconnect(conn);
		die("can't get default screen");
	}

	swidth = screen->width_in_pixels;
	sheight = screen->height_in_pixels;
	window = xcb_generate_id(conn);
	gc = xcb_generate_id(conn);
	px = xmalloc(swidth*sheight*sizeof(uint32_t));

	evmask = XCB_EVENT_MASK_EXPOSURE;

	xcb_create_window(
		conn, XCB_COPY_FROM_PARENT, window, screen->root,
		0, 0, 800, 600, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
		screen->root_visual, XCB_CW_EVENT_MASK, &evmask
	);

	xcb_create_gc(conn, gc, window, 0, 0);

	image = xcb_image_create_native(
		conn, swidth, sheight, XCB_IMAGE_FORMAT_Z_PIXMAP,
		screen->root_depth, px, sizeof(uint32_t)*swidth*sheight,
		(uint8_t *)(px)
	);

	/* set WM_NAME */
	xcb_change_property(
		conn, XCB_PROP_MODE_REPLACE, window,
		XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
		sizeof("fontdisplay") - 1, "fontdisplay"
	);

	/* set WM_CLASS */
	xcb_change_property(
		conn, XCB_PROP_MODE_REPLACE, window,
		XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8,
		sizeof("fontdisplay") * 2, "fontdisplay\0fontdisplay\0"
	);

	/* add WM_DELETE_WINDOW to WM_PROTOCOLS */
	xcb_change_property(
		conn, XCB_PROP_MODE_REPLACE, window,
		get_atom("WM_PROTOCOLS"), XCB_ATOM_ATOM, 32, 1,
		(const xcb_atom_t[]) { get_atom("WM_DELETE_WINDOW") }
	);

	/* set FULLSCREEN */
	xcb_change_property(
		conn, XCB_PROP_MODE_REPLACE, window,
		get_atom("_NET_WM_STATE"), XCB_ATOM_ATOM, 32, 1,
		(const xcb_atom_t[]) { get_atom("_NET_WM_STATE_FULLSCREEN") }
	);

	xcb_map_window(conn, window);
	xcb_flush(conn);
}

static void
destroy_window(void)
{
	xcb_free_gc(conn, gc);
	xcb_disconnect(conn);
	free(image);
	free(px);
}

static void
get_window_size(int16_t *width, int16_t *height)
{
	xcb_generic_error_t *error;
	xcb_get_geometry_cookie_t cookie;
	xcb_get_geometry_reply_t *reply;

	error = NULL;
	cookie = xcb_get_geometry(conn, window);
	reply = xcb_get_geometry_reply(conn, cookie, &error);

	if (NULL != error) {
		dief("xcb_get_geometry failed with error code: %d",
				(int)(error->error_code));
	}

	*width = reply->width;
	*height = reply->height;

	free(reply);
}

static void
h_client_message(xcb_client_message_event_t *ev)
{
	/* check if the wm sent a delete window message */
	/* https://www.x.org/docs/ICCCM/icccm.pdf */
	if (ev->data.data32[0] == get_atom("WM_DELETE_WINDOW")) {
		destroy_window();
		unload_font();
		exit(0);
	}
}

static void
h_expose(UNUSED xcb_expose_event_t *ev)
{
	int16_t width, height;
	size_t i, lines;
	int32_t y;

	get_window_size(&width, &height);
	xcb_clear_area(conn, 0, window, 0, 0, width, height);

	lines = sizeof(text) / sizeof(text[0]);
	y = (sheight - lines * ftheight) / 2;

	for (i = 0; i < lines; ++i) {
		render_string_centered(text[i], y);
		y += ftheight;
	}

	xcb_image_put(
		conn, window, gc, image,
		(width - swidth) / 2, (height - sheight) / 2, 0
	);
}

int
main(int argc, char **argv)
{
	xcb_generic_event_t *ev;
	const char *family = "Iosevka";

	if (++argv, --argc > 0) {
		if (!strcmp(*argv, "-h")) usage();
		else if (!strcmp(*argv, "-v")) version();
		else if (**argv == '-') dief("invalid option %s", *argv);
		else family = *argv;
	}

	create_window();
	load_font(family, 40);

	while ((ev = xcb_wait_for_event(conn))) {
		switch (ev->response_type & ~0x80) {
			case XCB_CLIENT_MESSAGE:
				h_client_message((xcb_client_message_event_t *)(ev));
				break;
			case XCB_EXPOSE:
				h_expose((xcb_expose_event_t *)(ev));
				break;
		}

		free(ev);
	}

	return 0;
}
