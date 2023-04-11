#include "shmem.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

int shmCreate(const char* name, int size) {
	printf("Uruchomiono funkcje tworzaca pamiec dzielona o nazwie %s i rozmiarze %d.\n", name, size);

	mode_t old_mask = umask(0);													/* zmiana maski, aby zostaly przyznane prawidlowe zezwolenia dostepu */
	enum {
		shm_access_permissions = 0666
	};
	enum {
		error = -1
	};
	int fd;																				/* deskryptor obiektu pamieci dzielonej */
	if((fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, shm_access_permissions)) == error) {										/* obsluga bledow shm_open() */
		printf("Blad w funkcji tworzacej pamiec dzielona o nazwie %s i rozmiarze %d. Pamiec dzielona o nazwie %s i rozmiarze %d nie zostala utworzona.\n", name, size, name, size);
		printf("Numer bledu: %d\n", errno);
		perror("shm_open error");
		exit(EXIT_FAILURE);
	}
	umask(old_mask);																			/* zmiana maski na pierwotna */

	if(ftruncate(fd, size) == error) {		/* obsluga bledow ftruncate() */
		printf("Numer bledu: %d\n", errno);
		perror("ftruncate error");
		exit(EXIT_FAILURE);
	}

	printf("Pamiec dzielona o nazwie %s i rozmiarze %d zostala utworzona. Jej deskryptor to: %d\n", name, size, fd);

	return fd;
}

int shmOpen(const char* name) {
	printf("Uruchomiono funkcje otwierajaca obiekt pamieci dzielonej %s.\n", name);

	mode_t old_mask = umask(0);													/* zmiana maski, aby zostaly przyznane prawidlowe zezwolenia dostepu */
	enum {
		shm_access_permissions = 0666
	};
	enum {
		error = -1
	};
	int fd;																/* deskryptor obiektu pamieci dzielonej */
	if((fd = shm_open(name, O_RDWR, shm_access_permissions)) == error) {								/* obsluga bledow shm_open() */
		printf("Blad w funkcji otwierajacej obiekt pamieci dzielonej %s. Pamiec dzielona %s nie zostala otwarta.\n", name);
		printf("Numer bledu: %d\n", errno);
		perror("shm_open error");
		exit(EXIT_FAILURE);
	}
	umask(old_mask);														/* zmiana maski na pierwotna */

	printf("Pamiec dzielona %s zostala otwarta. Jej deskryptor to: %d\n", name, fd);

	return fd;
}

void shmRm(const char* name) {
	printf("Uruchomiono funkcje usuwajaca obiekt pamieci dzielonej o nazwie %s.\n", name);

	enum {
		error = -1
	};
	if(shm_unlink(name) == error) {																/* obsluga bledow shm_unlink() */
		printf("Blad w funkcji usuwajacej obiekt pamieci dzielonej o nazwie %s. Pamiec dzielona o nazwie %s nie zostala usunieta.\n", name, name);
		printf("Numer bledu: %d\n", errno);
		perror("shm_unlink error");
		exit(EXIT_FAILURE);
	}

	printf("Pamiec dzielona o nazwie %s zostala usunieta.\n", name);
}

void* shmMap(int fd, int size) {
	printf("Uruchomiono funkcje odwzorowujaca obiekt pamieci dzielonej o deskryptorze %d i rozmiarze %d w przestrzen adresowa procesu.\n", fd, size);

	void *ptr;																/* adres poczatku odwzorowanego obszaru pamieci dzielonej */
	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(ptr == MAP_FAILED) {																	/* obsluga bledow mmap() */
		printf("Blad w funkcji odwzorowujacej obiekt pamieci dzielonej o deskryptorze %d i rozmiarze %d w przestrzen adresowa procesu. ", fd, size);
		printf("Obiekt pamieci dzielonej o deskryptorze %d i rozmiarze %d nie zostal odwzorowany w przestrzen adresowa procesu.\n", fd, size);
		printf("Numer bledu: %d\n", errno);
		perror("mmap error");
		exit(EXIT_FAILURE);
	}

	printf("Obiekt pamieci dzielonej o deskryptorze %d i rozmiarze %d zostal odwzorowany w przestrzen adresowa procesu. Adres poczatku odwzorowanego obszaru pamieci dzielonej to: %p\n", fd, size, ptr);

	return ptr;
}

void shmClose(void *ptr, int fd, int size) {
	printf("Uruchomiono funkcje usuwajaca odwzorowanie obszaru pamieci dzielonej o adresie %p oraz dlugosci %d i zamykajaca powiazany z obiektem deskryptor %d.\n", ptr, size, fd);

	enum {
		error = -1
	};
	if(munmap(ptr, size) == error) {												/* obsluga bledow munmap() */
		printf("Blad w funkcji usuwajacej odwzorowanie obszaru pamieci dzielonej o adresie %p oraz dlugosci %d. ", ptr, size);
		printf("Odwzorowanie obszaru pamieci dzielonej o adresie %p oraz dlugosci %d nie zostalo usuniete.\n", ptr, size);
		printf("Numer bledu: %d\n", errno);
		perror("munmap error");
		exit(EXIT_FAILURE);
	}

	if(close(fd) == error) {																/* obsluga bledow close() */
		printf("Blad w funkcji zamykajacej powiazany z obiektem deskryptor %d. Powiazany z obiektem deskryptor %d nie zostal zamkniety.\n", fd, fd);
		printf("Numer bledu: %d\n", errno);
		perror("close error");
		exit(EXIT_FAILURE);
	}

	printf("Odwzorowanie obszaru pamieci dzielonej o adresie %p oraz dlugosci %d zostalo usuniete. Powiazany z obiektem deskryptor %d zostal zamkniety.\n", ptr, size, fd);
}

int shmSize(int fd) {
	printf("Uruchomiono funkcje zwracajaca rozmiar pamieci dzielonej okreslonej deskryptorem %d.\n", fd);

	struct stat statbuf;	/* struktura z informacja na temat obiektu pamieci dzielonej */
	enum {
		error = -1
	};
	if(fstat(fd, &statbuf) == error) {																	/* obsluga bledow fstat() */
		printf("Blad w funkcji zwracajacej rozmiar pamieci dzielonej okreslonej deskryptorem %d, Rozmiar pamieci dzielonej okreslonej deskryptorem %d nie zostal zwrocony.\n", fd, fd);
		printf("Numer bledu: %d\n", errno);
		perror("fstat error");
		exit(EXIT_FAILURE);
	}

	printf("Rozmiar pamieci dzielonej okreslonej deskryptorem %d zostal zwrocony. Wynosi on: %d\n", fd, statbuf.st_size);

	return statbuf.st_size;
}

void shmInfo(int fd) {
	printf("Uruchomiono funkcje wypisujaca informacje o pamieci dzielonej okreslonej deskryptorem %d: prawa dostepu, rozmiar, wlasciciel, etc.\n", fd);

	struct stat statbuf;	/* struktura z informacja na temat obiektu pamieci dzielonej */
	enum {
		error = -1
	};
	if(fstat(fd, &statbuf) == error) {																	/* obsluga bledow fstat() */
		printf("Blad w funkcji wypisujacej informacje o pamieci dzielonej okreslonej deskryptorem %d. Informacje o pamieci dzielonej okreslonej deskryptorem %d nie zostaly wypisane.\n", fd, fd);
		printf("Numer bledu: %d\n", errno);
		perror("fstat error");
		exit(EXIT_FAILURE);
	}

	printf("Informacje o pamieci dzielonej okreslonej deskryptorem %d:\n", fd);

	printf("Prawa dostepu - ");
	printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
	printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
	printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
	printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
	printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
	printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
	printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
	printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
	printf((statbuf.st_mode & S_IXOTH) ? "x\n" : "-\n");

	printf("Rozmiar - %d.\n", statbuf.st_size);

	printf("ID uzytkownika wlasciciela - %d\n", statbuf.st_uid);
	printf("ID grupy wlasciciela - %d\n", statbuf.st_gid);
}