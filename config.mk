# Copyright (C) 2022-2024 <alpheratz99@protonmail.com>
# This program is free software.

VERSION = 2.0.1

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

PKG_CONFIG = pkg-config

DEPENDENCIES = fcft xcb xcb-shm xcb-image pixman-1

INCS = $(shell $(PKG_CONFIG) --cflags $(DEPENDENCIES)) -Iinclude
LIBS = $(shell $(PKG_CONFIG) --libs $(DEPENDENCIES))

CFLAGS = -Os $(INCS) -DVERSION=\"$(VERSION)\"
LDFLAGS = -s $(LIBS)

CC = cc
