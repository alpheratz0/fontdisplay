VERSION = 0.1.0
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man
LDLIBS = -lxcb -lfreetype -lxcb-image -lfontconfig
LDFLAGS = -s ${LDLIBS}
INCS = -I/usr/include -I/usr/include/freetype2
CFLAGS = -pedantic -Wall -Wextra -Os ${INCS} -DVERSION=\"${VERSION}\"
CC = cc

SRC = src/base/bitmap.c \
	  src/base/font.c \
	  src/ftds.c \
	  src/ui/fontset.c \
	  src/ui/label.c \
	  src/x11/window.c \
	  src/util/color.c \
	  src/util/debug.c

OBJ = ${SRC:.c=.o}

all: ftds

${OBJ}: src/base/font.h \
		src/base/bitmap.h \
		src/ui/fontset.h \
		src/ui/label.h \
		src/x11/window.h \
		src/x11/keys.h \
		src/util/debug.h \
		src/util/color.h \
		src/util/numdef.h

ftds: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

install: all
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f ftds ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/ftds
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@cp -f man/ftds.1 ${DESTDIR}${MANPREFIX}/man1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/ftds.1

dist: clean
	@mkdir -p ftds-${VERSION}
	@cp -R LICENSE Makefile README man src ftds-${VERSION}
	@tar -cf ftds-${VERSION}.tar ftds-${VERSION}
	@gzip ftds-${VERSION}.tar
	@rm -rf ftds-${VERSION}

uninstall:
	@rm -f ${DESTDIR}${PREFIX}/bin/ftds
	@rm -f ${DESTDIR}${MANPREFIX}/man1/ftds.1

clean:
	@rm -f ftds ftds-${VERSION}.tar.gz ${OBJ}

.PHONY: all clean install uninstall dist
