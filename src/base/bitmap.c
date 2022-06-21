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

#include "../util/numdef.h"
#include "../util/debug.h"
#include "bitmap.h"

extern bitmap_t *
bitmap_create(u32 width, u32 height, u32 color) {
	bitmap_t *bmp;

	if ((bmp = malloc(sizeof(bitmap_t)))) {
		bmp->width = width;
		bmp->height = height;
		if ((bmp->px = malloc(4*width*height))) {
			for (u32 i = 0; i < width * height; ++i) {
				bmp->px[i] = color;
			}
			return bmp;
		}
	}

	die("error while calling malloc, no memory available");

	return (void *)(0);
}

extern void
bitmap_set(bitmap_t *bmp, u32 x, u32 y, u32 color) {
	if (x < bmp->width && y < bmp->height) {
		bmp->px[y*bmp->width+x] = color;
	}
}

extern u32
bitmap_get(bitmap_t *bmp, u32 x, u32 y) {
	if (x < bmp->width && y < bmp->height) {
		return bmp->px[y*bmp->width+x];
	}
	return 0;
}

extern void
bitmap_rect(bitmap_t *bmp, u32 x, u32 y, u32 width, u32 height, u32 color) {
	for (u32 i = 0; i < width; ++i) {
		for (u32 j = 0; j < height; ++j) {
			bitmap_set(bmp, x + i, y + j, color);
		}
	}
}

extern void
bitmap_clear(bitmap_t *bmp, u32 color) {
	bitmap_rect(bmp, 0, 0, bmp->width, bmp->height, color);
}

extern void
bitmap_free(bitmap_t *bmp) {
	free(bmp->px);
	free(bmp);
}
