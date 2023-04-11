CC := gcc
CFLAGS := -O3 -Wall -std=c99 -c -pedantic -pthread
LDFLAGS := -s -pthread
LDLIBS := -lpthread

DIR = `basename $(CURDIR)`

.PHONY: all clean tar

all: piekarnia1.x piekarnia2.x

piekarnia1.o piekarnia2.o piekarnia.o: piekarnia.h

piekarnia1.x piekarnia2.x: piekarnia.o

%.x: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(wildcard *.[ox])

tar: clean
	(cd ../;tar -cvzf TomaszRaczkowski_$(DIR).tar.gz $(DIR))