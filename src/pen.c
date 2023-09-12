/*
	Copyright (C) 2023 <alpheratz99@protonmail.com>

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

#include <stdint.h>
#include <stdlib.h>
#include <pixman.h>

#include "utils.h"
#include "pen.h"

#define RED(c) ((c>>16) & 0xff)
#define GREEN(c) ((c>>8) & 0xff)
#define BLUE(c) ((c>>0) & 0xff)

extern Pen_t *
pen_new(uint32_t c)
{
	Pen_t *pen;

	pen = xmalloc(sizeof(Pen_t));

	pen->img = pixman_image_create_solid_fill(
		&(const pixman_color_t) {
			.red = RED(c) * 257,
			.green = GREEN(c) * 257,
			.blue = BLUE(c) * 257,
			.alpha = 0xff
		}
	);

	return pen;
}

extern void
pen_free(Pen_t *pen)
{
	pixman_image_unref(pen->img);
	free(pen);
}
