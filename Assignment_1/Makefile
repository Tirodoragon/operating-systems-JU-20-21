CC := gcc
CFLAGS := -O3 -Wall -std=c99 -c
LDFLAGS := -s
LDLIBS :=

DIR = `basename $(CURDIR)`

.PHONY: all clean tar

all: main.x kopiuj.x procesy.x lider.x b.x a.x

main.o procesy.o lider.o b.o procinfo.o: procinfo.h

main.x procesy.x lider.x b.x: procinfo.o

%.x: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(wildcard *.[ox])

tar: clean
	(cd ../; tar -cvzf TomaszRaczkowski_$(DIR).tar.gz $(DIR))