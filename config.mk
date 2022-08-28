# Copyright (C) 2022 <alpheratz99@protonmail.com>
# This program is free software.

VERSION   = 1.0.1

CC        = cc
INCS      = -I/usr/include/freetype2
CFLAGS    = -std=c99 -pedantic -Wall -Wextra -Os $(INCS) -DVERSION=\"$(VERSION)\"
LDLIBS    = -lxcb -lfreetype -lxcb-image -lfontconfig
LDFLAGS   = -s

PREFIX    = /usr/local
MANPREFIX = $(PREFIX)/share/man
