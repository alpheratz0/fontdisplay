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

#include "numdef.h"
#include "color.h"

static u8
blerp(u8 from, u8 to, u8 v)
{
	return from + ((to - from) * v) / 0xff;
}

extern u32
color_lerp(u32 from, u32 to, u8 v)
{
	u8 r = blerp((from >> 16) & 0xff, (to >> 16) & 0xff, v);
	u8 g = blerp((from >> 8) & 0xff, (to >> 8) & 0xff, v);
	u8 b = blerp(from & 0xff, to & 0xff, v);

	return (r << 16) | (g << 8) | b;
}
