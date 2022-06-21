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

#ifndef __FONTDISPLAY_BASE_BITMAP_H__
#define __FONTDISPLAY_BASE_BITMAP_H__

#include "../util/numdef.h"

typedef struct bitmap bitmap_t;

struct bitmap {
	u32 *px;
	u32 width;
	u32 height;
};

extern bitmap_t *
bitmap_create(u32 width, u32 height, u32 color);

extern void
bitmap_set(bitmap_t *bmp, u32 x, u32 y, u32 color);

extern u32
bitmap_get(bitmap_t *bmp, u32 x, u32 y);

extern void
bitmap_rect(bitmap_t *bmp, u32 x, u32 y, u32 width, u32 height, u32 color);

extern void
bitmap_clear(bitmap_t *bmp, u32 color);

extern void
bitmap_free(bitmap_t *bmp);

#endif
