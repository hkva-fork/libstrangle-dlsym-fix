CC=gcc
CFLAGS=-rdynamic -fPIC -D_GNU_SOURCE -shared -Wall
prefix=/usr/local
bindir=$(prefix)/bin
libdir=$(prefix)/lib
LIB32_PATH=$(libdir)/libstrangle/lib32
LIB64_PATH=$(libdir)/libstrangle/lib64

all: libstrangle64.so libstrangle32.so libstrangle.conf

libstrangle.conf:
	@echo "$(LIB32_PATH)/" > libstrangle.conf
	@echo "$(LIB64_PATH)/" >> libstrangle.conf

libstrangle64.so:
	$(CC) $(CFLAGS) -m64 -o libstrangle64.so libstrangle.c

libstrangle32.so:
	$(CC) $(CFLAGS) -m32 -o libstrangle32.so libstrangle.c

install: all
	install -m 0644 -D -T libstrangle.conf $(DESTDIR)/etc/ld.so.conf.d/libstrangle.conf
	install -m 0755 -D -T libstrangle64.so $(DESTDIR)$(LIB32_PATH)/libstrangle.so
	install -m 0755 -D -T libstrangle32.so $(DESTDIR)$(LIB64_PATH)/libstrangle.so
	install -m 0755 -D -T strangle.sh $(DESTDIR)$(bindir)/strangle
	ldconfig

clean:
	rm -f libstrangle64.so
	rm -f libstrangle32.so
	rm -f libstrangle.conf

uninstall:
	rm -f $(DESTDIR)/etc/ld.so.conf.d/libstrangle.conf
	rm -f $(DESTDIR)$(LIB32_PATH)/libstrangle.so
	rm -f $(DESTDIR)$(LIB64_PATH)/libstrangle.so
	rm -f $(DESTDIR)$(bindir)/strangle