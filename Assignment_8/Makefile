CC := gcc
CFLAGS := -O3 -Wall -std=c99 -c -pedantic
LDFLAGS := -s -L.
LDLIBS := -lmsg -lrt
CFLAGSLIB := -c -O -fPIC
LDFLAGSLIB = -shared -o
AR := ar
ARFLAGS := rv

DIR = `basename $(CURDIR)`

.PHONY: all clean tar

all: libmsg.a libmsg.so serwer.x klient.x

serwer.x klient.x msg.o: msg.h

serwer.x klient.x: libmsg.so

msg.o: msg.c
	$(CC) $(CFLAGSLIB) $<

libmsg.so: msg.o
	$(CC) $(LDFLAGSLIB) $@ $<

libmsg.a: msg.o
	$(AR) $(ARFLAGS) $@ $<

%.x: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) -Wl,-R .

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(wildcard *.[aox])
	$(RM) libmsg.so

tar: clean
	(cd ../;tar -cvzf TomaszRaczkowski_$(DIR).tar.gz $(DIR))