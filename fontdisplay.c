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

#define CHKFTERR(name,err) do {                                 \
	FT_Error error;                                             \
	error = (err);                                              \
	if (error != 0)                                             \
		die(name " failed with error code: %d", (int)(error)); \
} while (0)

/* X11 */
static xcb_connection_t *conn;
static xcb_screen_t *screen;
static xcb_window_t window;
static xcb_gcontext_t gc;
static xcb_image_t *image;
static int32_t wwidth, wheight;
static uint32_t *wpx;

/* FT */
static FT_Library ftlib;
static FT_Face ftface;
static uint32_t ftsize;
static uint32_t ftwidth, ftheight;

static uint32_t tcolor = 0xada54e;
static uint32_t *tpx;
static int32_t twidth, theight;
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
die(const char *fmt, ...)
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
xcalloc(size_t n, size_t size)
{
	void *p;

	if (NULL == (p = calloc(n, size)))
		die("error while calling calloc, no memory available");

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

	if (NULL == (path = search_font(family)))
		die("font family not found: %s", family);

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
	int32_t gwidth, gheight, xmap, ymap, gray, i, j;

	CHKFTERR("FT_Load_Char", FT_Load_Char(ftface, c, FT_LOAD_RENDER));

	glyph = ftface->glyph;
	gheight = glyph->bitmap.rows;
	gwidth = glyph->bitmap.width;

	for (i = 0; i < gheight; ++i) {
		for (j = 0; j < gwidth; ++j) {
			xmap = x + j + glyph->bitmap_left;
			ymap = y + i - glyph->bitmap_top + ftsize;
			gray = glyph->bitmap.buffer[i*gwidth+j];

			if (ymap < theight && xmap < twidth)
				tpx[ymap*twidth+xmap] = color_lerp(0, tcolor, gray);
		}
	}
}

static xcb_atom_t
get_atom(const char *name)
{
	xcb_atom_t atom;
	xcb_generic_error_t *error;
	xcb_intern_atom_cookie_t cookie;
	xcb_intern_atom_reply_t *reply;

	cookie = xcb_intern_atom(conn, 0, strlen(name), name);
	reply = xcb_intern_atom_reply(conn, cookie, &error);

	if (NULL != error)
		die("xcb_intern_atom failed with error code: %d",
				(int)(error->error_code));

	atom = reply->atom;
	free(reply);

	return atom;
}

static void
create_window(void)
{
	if (xcb_connection_has_error(conn = xcb_connect(NULL, NULL)))
		die("can't open display");

	if (NULL == (screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data))
		die("can't get default screen");

	wwidth = screen->width_in_pixels;
	wheight = screen->height_in_pixels;
	wpx = xcalloc(wwidth * wheight, sizeof(uint32_t));

	window = xcb_generate_id(conn);
	gc = xcb_generate_id(conn);

	xcb_create_window_aux(
		conn, screen->root_depth, window, screen->root, 0, 0,
		wwidth, wheight, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
		screen->root_visual, XCB_CW_EVENT_MASK,
		(const xcb_create_window_value_list_t []) {{
			.event_mask = XCB_EVENT_MASK_EXPOSURE |
			              XCB_EVENT_MASK_STRUCTURE_NOTIFY
		}}
	);

	xcb_create_gc(conn, gc, window, 0, NULL);

	image = xcb_image_create_native(
		conn, wwidth, wheight, XCB_IMAGE_FORMAT_Z_PIXMAP, screen->root_depth,
		wpx, sizeof(uint32_t) * wwidth * wheight, (uint8_t *)(wpx)
	);

	xcb_change_property(
		conn, XCB_PROP_MODE_REPLACE, window, get_atom("_NET_WM_NAME"),
		get_atom("UTF8_STRING"), 8, sizeof("fontdisplay") - 1, "fontdisplay"
	);

	xcb_change_property(
		conn, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_CLASS,
		XCB_ATOM_STRING, 8, sizeof("fontdisplay") * 2, "fontdisplay\0fontdisplay\0"
	);

	xcb_change_property(
		conn, XCB_PROP_MODE_REPLACE, window,
		get_atom("WM_PROTOCOLS"), XCB_ATOM_ATOM, 32, 1,
		(const xcb_atom_t []) { get_atom("WM_DELETE_WINDOW") }
	);

	xcb_change_property(
		conn, XCB_PROP_MODE_REPLACE, window,
		get_atom("_NET_WM_STATE"), XCB_ATOM_ATOM, 32, 1,
		(const xcb_atom_t []) { get_atom("_NET_WM_STATE_FULLSCREEN") }
	);

	xcb_map_window(conn, window);
	xcb_flush(conn);
}

static void
destroy_window(void)
{
	xcb_free_gc(conn, gc);
	xcb_image_destroy(image);
	xcb_disconnect(conn);
}

static void
prerender_text(void)
{
	size_t i, n, len, lines, line_max_width;
	uint32_t y, x;

	lines = sizeof(text) / sizeof(text[0]);
	theight = lines * ftheight;
	line_max_width = 0;

	for (i = 0; i < lines; ++i)
		if (strlen(text[i]) > line_max_width)
			line_max_width = strlen(text[i]);

	twidth = line_max_width * ftwidth + ftwidth * 5;

	tpx = xcalloc(theight * twidth, sizeof(uint32_t));
	y = 0;

	for (i = 0; i < lines; ++i) {
		len = strlen(text[i]);
		x = (twidth - len * ftwidth) / 2;

		for (n = 0; n < len && text[i][n] != '\n'; ++n)
			render_char(text[i][n], x + n * ftwidth, y);

		y += ftheight;
	}
}

static void
destroy_prerendered_text(void)
{
	free(tpx);
}

static void
prepare_render(void)
{
	int32_t x, y, ox, oy;

	memset(wpx, 0, sizeof(uint32_t) * wwidth * wheight);

	ox = (wwidth - twidth) / 2;
	oy = (wheight - theight) / 2;

	for (y = 0; y < theight; ++y) {
		if ((y+oy) < 0 || (y+oy) >= wheight)
			continue;
		for (x = 0; x < twidth; ++x) {
			if ((x+ox) < 0 || (x+ox) >= wwidth)
				continue;
			wpx[(y+oy)*wwidth+(x+ox)] = tpx[y*twidth+x];
		}
	}
}

static void
h_client_message(xcb_client_message_event_t *ev)
{
	/* check if the wm sent a delete window message */
	/* https://www.x.org/docs/ICCCM/icccm.pdf */
	if (ev->data.data32[0] == get_atom("WM_DELETE_WINDOW")) {
		destroy_window();
		destroy_prerendered_text();
		unload_font();
		exit(0);
	}
}

static void
h_expose(UNUSED xcb_expose_event_t *ev)
{
	xcb_image_put(conn, window, gc, image, 0, 0, 0);
}

static void
h_configure_notify(xcb_configure_notify_event_t *ev)
{
	if (wwidth == ev->width && wheight == ev->height)
		return;

	xcb_image_destroy(image);

	wwidth = ev->width;
	wheight = ev->height;
	wpx = xcalloc(wwidth * wheight, sizeof(uint32_t));

	image = xcb_image_create_native(
		conn, wwidth, wheight, XCB_IMAGE_FORMAT_Z_PIXMAP, screen->root_depth,
		wpx, sizeof(uint32_t) * wwidth * wheight, (uint8_t *)(wpx)
	);

	prepare_render();
	xcb_image_put(conn, window, gc, image, 0, 0, 0);
	xcb_flush(conn);
}

int
main(int argc, char **argv)
{
	xcb_generic_event_t *ev;
	const char *family;
	
	family = NULL;

	while (++argv, --argc > 0) {
		if ((*argv)[0] == '-' && (*argv)[1] != '\0' && (*argv)[2] == '\0') {
			switch ((*argv)[1]) {
				case 'h': usage(); break;
				case 'v': version(); break;
				default: die("invalid option %s", *argv); break;
			}
		} else {
			if (NULL != family)
				die("unexpected argument: %s", *argv);
			family = *argv;
		}
	}

	if (NULL == family)
		family = "Iosevka";

	create_window();
	load_font(family, 40);
	prerender_text();
	prepare_render();

	while ((ev = xcb_wait_for_event(conn))) {
		switch (ev->response_type & ~0x80) {
			case XCB_CLIENT_MESSAGE:     h_client_message((void *)(ev)); break;
			case XCB_EXPOSE:             h_expose((void *)(ev)); break;
			case XCB_CONFIGURE_NOTIFY:   h_configure_notify((void *)(ev)); break;
		}

		free(ev);
	}

	return 0;
}
