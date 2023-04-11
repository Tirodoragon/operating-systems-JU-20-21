CC := gcc
CFLAGS := -O3 -Wall -std=c99 -c -pedantic
LDFLAGS := -s -L.
LDFLAGS3 := -s -pthread -L.
LDLIBS := -lshmem -lrt
LDLIBS3 := -lsem -lshmem -lrt
CFLAGSLIBSHM := -c -O -fPIC
CFLAGSLIBSEM := -c
LDFLAGSLIB = -shared -o
AR := ar
ARFLAGS := rv

DIR = `basename $(CURDIR)`

.PHONY: all clean tar

all: libsem.a libshmem.so shmuse1.x shmuse2.x glowny.x producent.x konsument.x

shmuse1.x shmuse2.x glowny.x producent.x konsument.x shmem.o: shmem.h

glowny.x producent.x konsument.x sem.o: sem.h

shmuse1.x shmuse2.x glowny.x producent.x konsument.x: libshmem.so

glowny.x producent.x konsument.x: libsem.a

shmem.o: shmem.c
	$(CC) $(CFLAGSLIBSHM) $<

libshmem.so: shmem.o
	$(CC) $(LDFLAGSLIB) $@ $<

sem.o: sem.c
	$(CC) $(CFLAGSLIBSEM) $<

libsem.a: sem.o
	$(AR) $(ARFLAGS) $@ $<

shmuse1.x: shmuse1.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) -Wl,-R .

shmuse2.x: shmuse2.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.x: %.o
	$(CC) $(LDFLAGS3) -o $@ $^ $(LDLIBS3) -Wl,-R .

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(wildcard *.[aox])
	$(RM) libshmem.so
	$(RM) schowek.txt

tar: clean
	(cd ../;tar -cvzf TomaszRaczkowski_$(DIR).tar.gz $(DIR))