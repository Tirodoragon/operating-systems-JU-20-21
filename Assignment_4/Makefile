CC := gcc
CFLAGS := -O3 -Wall -std=c99 -c -pedantic
LDFLAGS := -s
LDLIBS :=

DIR = `basename $(CURDIR)`

.PHONY: all clean tar

all: prodkons.x

%.x: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(wildcard *.[ox])
	$(RM) schowek.txt

tar: clean
	(cd ../;tar -cvzf TomaszRaczkowski_$(DIR).tar.gz $(DIR))