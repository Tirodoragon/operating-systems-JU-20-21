CC := gcc
CFLAGS := -O3 -Wall -std=c99 -c -pedantic
LDFLAGS := -s -pthread -L.
LDLIBS := -lsem -lrt
CFLAGSLIB := -c
AR := ar
ARFLAGS := rv

DIR = `basename $(CURDIR)`

.PHONY: all clean tar

all: libsem.a semuse.x powielacz.x prog.x

semuse.o prog.o powielacz.o libsam.a sem.o: sem.h

semuse.x prog.x powielacz.x: libsem.a

sem.o: sem.c
	$(CC) $(CFLAGSLIB) $<

libsem.a: sem.o
	$(AR) $(ARFLAGS) $@ $<

%.x: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(wildcard *.[aox])
	$(RM) numer.txt

tar: clean
	(cd ../;tar -cvzf TomaszRaczkowski_$(DIR).tar.gz $(DIR))