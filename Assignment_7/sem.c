#include "sem.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

semid semCreate(const char *name, int val) {
	printf("Uruchomiono funkcje tworzaca semafor %s z wartoscia poczatkowa %d.\n", name, val);

	enum {
		sem_access_permissions = 0644
	};
	semid sem_address;
	if((sem_address = sem_open(name, O_CREAT | O_EXCL, sem_access_permissions, val)) == SEM_FAILED) {									/* obsluga bledow sem_open() */
		printf("Blad w funkcji tworzacej semafor %s z wartoscia poczatkowa %d. Semafor %s z wartoscia poczatkowa %d nie zostal utworzony.\n", name, val, name, val);
		printf("Numer bledu: %d\n", errno);
		perror("sem_open error");
		exit(EXIT_FAILURE);
	}

	printf("Semafor %s z wartoscia poczatkowa %d zostal utworzony. Jego adres to: %p\n", name, val, sem_address);

	return sem_address;
}

void semRemove(const char *name) {
	printf("Uruchomiono funkcje usuwajaca semafor %s.\n", name);

	enum {
		error = -1
	};
	if(sem_unlink(name) == error) {										/* obsluga bledow sem_unlink() */
		printf("Blad w funkcji usuwajacej semafor %s. Semafor %s nie zostal usuniety.\n", name, name);
		printf("Numer bledu: %d\n", errno);
		perror("sem_unlink error");
		exit(EXIT_FAILURE);
	}

	printf("Semafor %s zostal usuniety.\n", name);
}

semid semOpen(const char *name) {
	printf("Uruchomiono funkcje uzyskujaca dostep do istniejacego semafora %s.\n", name);

	semid sem_address;
	if((sem_address = sem_open(name, O_CREAT)) == SEM_FAILED) {											/* obsluga bledow sem_open() */
		printf("Blad w funkcji uzyskujacej dostep do istniejacego semafora %s. Istniejacy semafor %s nie zostal otworzony.\n", name, name);
		printf("Numer bledu: %d\n", errno);
		perror("sem_open error");
		exit(EXIT_FAILURE);
	}

	printf("Istniejacy semafor %s zostal otwarty. Jego adres to: %p\n", name, sem_address);

	return sem_address;
}

void semClose(semid sem) {
	printf("Uruchomiono funkcje zamykajaca semafor o adresie %p w danym procesie.\n", sem);

	enum {
		error = -1
	};
	if(sem_close(sem) == error) {																	/* obsluga bledow sem_close() */
		printf("Blad w funkcji zamykajacej semafor o adresie %p w danym procesie. Semafor o adresie %p w danym procesie nie zostal zamkniety.\n", sem, sem);
		printf("Numer bledu: %d\n", errno);
		perror("sem_close error");
		exit(EXIT_FAILURE);
	}

	printf("Semafor o adresie %p w danym procesie zostal zamkniety.\n", sem);
}

void semP(semid sem) {
	printf("Uruchomiono funkcje opuszczajaca semafor o adresie %p.\n", sem);

	enum {
		error = -1
	};
	if(sem_wait(sem) == error) {													/* obsluga bledow sem_wait() */
		printf("Blad w funkcji opuszczajacej semafor o adresie %p. Semafor o adresie %p nie zostal opuszczony.\n", sem, sem);
		printf("Numer bledu: %d\n", errno);
		perror("sem_wait error");
		exit(EXIT_FAILURE);
	}

	printf("Semafor o adresie %p zostal opuszczony.\n", sem);
}

void semV(semid sem) {
	printf("Uruchomiono funkcje podnoszaca semafor o adresie %p.\n", sem);

	enum {
		error = -1
	};
	if(sem_post(sem) == error) {													/* obsluga bledow sem_post() */
		printf("Blad w funkcji podnoszacej semafor o adresie %p. Semafor o adresie %p nie zostal podniesiony.\n", sem, sem);
		printf("Numer bledu: %d\n", errno);
		perror("sem_post error");
		exit(EXIT_FAILURE);
	}

	printf("Semafor o adresie %p zostal podniesiony.\n", sem);
}

void semInfo(semid sem) {
	printf("Uruchomiono funkcje wypisujaca informacje o wartosci semafora o adresie %p.\n", sem);

	int sval;
	enum {
		error = -1
	};
	if(sem_getvalue(sem, &sval) == error) {																/* obsluga bledow sem_getvalue() */
		printf("Blad w funkcji wypisujacej informacje o wartosci semafora o adresie %p. Nie wypisano informacji o wartosci semafora o adresie %p.", sem, sem);
		printf("Numer bledu: %d\n", errno);
		perror("sem_getvalue error");
		exit(EXIT_FAILURE);
	}

	printf("Wartosc semafora o adresie %p to: %d\n", sem, sval);
}