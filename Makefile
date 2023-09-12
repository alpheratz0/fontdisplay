.POSIX:
.PHONY: all clean install uninstall dist

include config.mk

OBJ=\
	src/fontdisplay.o \
	src/pixbuf.o \
	src/log.o \
	src/pen.o \
	src/text-renderer.o \
	src/utils.o

all: fontdisplay

fontdisplay: $(OBJ)
	$(CC) $(OBJ) -o fontdisplay $(LDFLAGS)

clean:
	rm -f fontdisplay $(OBJ) fontdisplay-$(VERSION).tar.gz

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f fontdisplay $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/fontdisplay
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f fontdisplay.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/fontdisplay.1

dist: clean
	mkdir -p fontdisplay-$(VERSION)
	cp -R COPYING config.mk Makefile README fontdisplay.1 src include \
		fontdisplay-$(VERSION)
	tar -cf fontdisplay-$(VERSION).tar fontdisplay-$(VERSION)
	gzip fontdisplay-$(VERSION).tar
	rm -rf fontdisplay-$(VERSION)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/fontdisplay
	rm -f $(DESTDIR)$(MANPREFIX)/man1/fontdisplay.1
