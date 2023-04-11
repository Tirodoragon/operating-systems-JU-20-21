CC := gcc
CFLAGS := -O3 -Wall -std=c99 -c
LDFLAGS := -s
LDLIBS :=

DIR = `basename $(CURDIR)`

.PHONY: all clean tar

all: obsluga.x wysylaj.x grupa.x

obsluga.o wysylaj.o grupa.o procinfo.o: procinfo.h

obsluga.x wysylaj.x grupa.x: procinfo.o

%.x: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(wildcard *.[ox])

tar: clean
	(cd ../;tar -cvzf TomaszRaczkowski_$(DIR).tar.gz $(DIR))