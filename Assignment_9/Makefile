CC := gcc
CFLAGS := -O3 -Wall -std=c99 -c -pedantic -pthread
LDFLAGS := -s -pthread
LDLIBS := -lpthread

DIR = `basename $(CURDIR)`

.PHONY: all clean tar

all: wykluczanie.x

%.x: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(wildcard *.[ox])

tar: clean
	(cd ../;tar -cvzf TomaszRaczkowski_$(DIR).tar.gz $(DIR))