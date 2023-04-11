CC := gcc
CFLAGS := -O3 -Wall -std=c99 -c
LDFLAGS := -s
LDLIBS :=

DIR = `basename $(CURDIR)`

.PHONY: all clean tar

all: drzewko1.x drzewko2.x macierzysty.x potomny.x execute.x zombi.x orphan.x strlen.x

drzewko1.o drzewko2.o macierzysty.o potomny.o procinfo.o: procinfo.h

drzewko1.x drzewko2.x macierzysty.x potomny.x: procinfo.o

%.x: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(wildcard *.[ox])

tar: clean
	(cd ../; tar -cvzf TomaszRaczkowski_$(DIR).tar.gz $(DIR))