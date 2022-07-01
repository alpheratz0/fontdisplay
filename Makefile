VERSION = 1.0.1-rev+${shell git rev-parse --short=16 HEAD}
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man
LDLIBS = -lxcb -lfreetype -lxcb-image -lfontconfig
LDFLAGS = -s ${LDLIBS}
INCS = -I/usr/include -I/usr/include/freetype2
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Os ${INCS} -DVERSION="\"${VERSION}\""
CC = cc

SRC = src/base/bitmap.c \
	  src/base/font.c \
	  src/fontdisplay.c \
	  src/ui/fontset.c \
	  src/ui/label.c \
	  src/x11/window.c \
	  src/util/color.c \
	  src/util/debug.c \
	  src/util/xmalloc.c

OBJ = ${SRC:.c=.o}

all: fontdisplay

${OBJ}: src/base/font.h \
		src/base/bitmap.h \
		src/ui/fontset.h \
		src/ui/label.h \
		src/x11/window.h \
		src/x11/keys.h \
		src/util/debug.h \
		src/util/color.h \
		src/util/xmalloc.h

fontdisplay: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f fontdisplay ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/fontdisplay
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	cp -f man/fontdisplay.1 ${DESTDIR}${MANPREFIX}/man1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/fontdisplay.1

dist: clean
	mkdir -p fontdisplay-${VERSION}
	cp -R LICENSE Makefile README man src fontdisplay-${VERSION}
	tar -cf fontdisplay-${VERSION}.tar fontdisplay-${VERSION}
	gzip fontdisplay-${VERSION}.tar
	rm -rf fontdisplay-${VERSION}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/fontdisplay
	rm -f ${DESTDIR}${MANPREFIX}/man1/fontdisplay.1

clean:
	rm -f fontdisplay fontdisplay-${VERSION}.tar.gz ${OBJ}

.PHONY: all clean install uninstall dist
