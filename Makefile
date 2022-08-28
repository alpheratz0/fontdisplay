.POSIX:
.PHONY: all clean install uninstall dist

include config.mk

all: fontdisplay

fontdisplay: fontdisplay.o
	$(CC) $(LDFLAGS) -o fontdisplay fontdisplay.o $(LDLIBS)

clean:
	rm -f fontdisplay fontdisplay.o fontdisplay-$(VERSION).tar.gz

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f fontdisplay $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/fontdisplay
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f fontdisplay.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/fontdisplay.1

dist: clean
	mkdir -p fontdisplay-$(VERSION)
	cp -R COPYING config.mk Makefile README fontdisplay.1 fontdisplay.c fontdisplay-$(VERSION)
	tar -cf fontdisplay-$(VERSION).tar fontdisplay-$(VERSION)
	gzip fontdisplay-$(VERSION).tar
	rm -rf fontdisplay-$(VERSION)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/fontdisplay
	rm -f $(DESTDIR)$(MANPREFIX)/man1/fontdisplay.1
