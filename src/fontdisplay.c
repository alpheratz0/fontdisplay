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

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <sys/file.h>

#include "pen.h"
#include "utils.h"
#include "text-renderer.h"
#include "pixbuf.h"
#include "log.h"

#define LINE_MARGIN 10

static Pixbuf_t *pb;
static Pen_t *white_pen;
static TextRenderer_t *text_renderer;
static xcb_connection_t *conn;
static xcb_screen_t *scr;
static xcb_window_t win;
static bool should_close;

static const char *text[] = {
	"0 1 2 3 4 5 6 7 8 9",
	"a b c d e f g h i j k l m",
	"n o p q r s t u v w x y z",
	"A  B  C  D  E  F  G  H  I  J  K  L  M",
	"N  O  P  Q  R  S  T  U  V  W  X  Y  Z",
	"~ ! @ # $ % ^ & _ \" ` ' | : , . ?",
	"- + * / \\ = [ ] ( ) { } < > ;"
};

static xcb_atom_t
get_x11_atom(const char *name)
{
	xcb_atom_t atom;
	xcb_generic_error_t *error;
	xcb_intern_atom_cookie_t cookie;
	xcb_intern_atom_reply_t *reply;

	cookie = xcb_intern_atom(conn, 0, strlen(name), name);
	reply = xcb_intern_atom_reply(conn, cookie, &error);

	if (NULL != error)
		die("xcb_intern_atom failed with error code: %hhu",
				error->error_code);

	atom = reply->atom;
	free(reply);

	return atom;
}

static void
xwininit(void)
{
	const char *wm_class,
		       *wm_name;

	xcb_atom_t _NET_WM_NAME;

	xcb_atom_t WM_PROTOCOLS,
			   WM_DELETE_WINDOW;

	xcb_atom_t _NET_WM_STATE,
			   _NET_WM_STATE_FULLSCREEN;

	xcb_atom_t UTF8_STRING;

	conn = xcb_connect(NULL, NULL);

	if (xcb_connection_has_error(conn))
		die("can't open display");

	scr = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;

	if (NULL == scr)
		die("can't get default screen");

	win = xcb_generate_id(conn);

	xcb_create_window_aux(
		conn, scr->root_depth, win, scr->root, 0, 0,
		800, 600, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
		scr->root_visual, XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
		(const xcb_create_window_value_list_t []) {{
			.background_pixel = 0x000000,
			.event_mask = XCB_EVENT_MASK_EXPOSURE |
			              XCB_EVENT_MASK_STRUCTURE_NOTIFY
		}}
	);

	_NET_WM_NAME = get_x11_atom("_NET_WM_NAME");
	UTF8_STRING = get_x11_atom("UTF8_STRING");
	wm_name = "fontdisplay";

	xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win,
		_NET_WM_NAME, UTF8_STRING, 8, strlen(wm_name), wm_name);

	wm_class = "fontdisplay\0fontdisplay\0";
	xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win, XCB_ATOM_WM_CLASS,
		XCB_ATOM_STRING, 8, strlen(wm_class), wm_class);

	WM_PROTOCOLS = get_x11_atom("WM_PROTOCOLS");
	WM_DELETE_WINDOW = get_x11_atom("WM_DELETE_WINDOW");

	xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win,
		WM_PROTOCOLS, XCB_ATOM_ATOM, 32, 1, &WM_DELETE_WINDOW);

	_NET_WM_STATE = get_x11_atom("_NET_WM_STATE");
	_NET_WM_STATE_FULLSCREEN = get_x11_atom("_NET_WM_STATE_FULLSCREEN");

	xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win,
		_NET_WM_STATE, XCB_ATOM_ATOM, 32, 1, &_NET_WM_STATE_FULLSCREEN);

	pb = pixbuf_new(conn, win, scr->width_in_pixels, scr->height_in_pixels);

	xcb_map_window(conn, win);
	xcb_flush(conn);
}

static void
xwindestroy(void)
{
	pixbuf_free(pb);
	xcb_destroy_window(conn, win);
	xcb_disconnect(conn);
}

static void
h_client_message(xcb_client_message_event_t *ev)
{
	xcb_atom_t WM_DELETE_WINDOW;

	WM_DELETE_WINDOW = get_x11_atom("WM_DELETE_WINDOW");

	/* check if the wm sent a delete window message */
	/* https://www.x.org/docs/ICCCM/icccm.pdf */
	if (ev->data.data32[0] == WM_DELETE_WINDOW)
		should_close = true;
}

static void
h_expose(UNUSED xcb_expose_event_t *ev)
{
	pixbuf_render(pb);
}

static void
h_configure_notify(xcb_configure_notify_event_t *ev)
{
	pixbuf_set_container_size(pb, ev->width, ev->height);
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

int
main(int argc, char **argv)
{
	const char *family;
	xcb_generic_event_t *ev;
	size_t i, lines, x, y;

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

	xwininit();

	white_pen = pen_new(0xffffff);
	text_renderer = text_renderer_new(family, 40);

	lines = sizeof(text) / sizeof(text[0]);

	y = pixbuf_get_height(pb);
	y = y - lines * text_renderer_text_height(text_renderer);
	y = y - (lines - 1) * LINE_MARGIN;
	y = y / 2;

	for (i = 0; i < lines; ++i) {
		x = (pixbuf_get_width(pb) - text_renderer_text_width(text_renderer, text[i])) / 2;
		text_renderer_draw_string(text_renderer, pb, white_pen, text[i], x, y);
		y += text_renderer_text_height(text_renderer) + LINE_MARGIN;
	}

	while (!should_close && (ev = xcb_wait_for_event(conn))) {
		switch (ev->response_type & ~0x80) {
		case XCB_CLIENT_MESSAGE:     h_client_message((void *)(ev)); break;
		case XCB_EXPOSE:             h_expose((void *)(ev)); break;
		case XCB_CONFIGURE_NOTIFY:   h_configure_notify((void *)(ev)); break;
		}

		free(ev);
	}

	pen_free(white_pen);
	text_renderer_free(text_renderer);
	xwindestroy();

	return 0;
}
