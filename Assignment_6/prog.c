#define _DEFAULT_SOURCE

#include "sem.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	enum {
		error = -1
	};

	int fd;	/* deskryptor pliku */

	enum {
		size = 12
	};
	char buffer[size];

	enum {
		slash_max = 251		/* maksymalna ilosc znakow w nazwie semafora z ukosnikiem */
	};
	char slash_name[slash_max];	/* nazwa semafora z ukosnikiem */

	strcpy(slash_name, argv[3]);

	semid sem_address = semOpen(slash_name);

	int liczba_sekcji_krytycznych = atoi(argv[1]);
	pid_t pid = getpid();
	int sem_value = atoi(argv[2]);
	srand(time(NULL));	/* seed z czasu biezacego, aby przy kazdym uruchomieniu programu byly rozne wartosci rand() */
	int min = 1;		/* przedzial generowanych wartosci przez rand() */
	int max = 1000000;	/* 1 zeby sleep trwal minimum jedna mikrosekunde, a 1000000 poniewaz powyzej tej liczby w niektorych systemach jest to blad w usleep() */
	int numer;
	int snprintf_return;
	for(int i = 1; i <= liczba_sekcji_krytycznych; i++) {
		printf("Przed sekcja krytyczna numer %d: pid - %d, wartosc semafora - %d\n", i, pid, sem_value);

		usleep(rand() % (max + 1 - min) + min);	/* symulowanie roznych szybkosci dzialania programu */

		semP(sem_address);

		sem_value -= 1;

		printf("	W trakcie sesji krytycznej numer %d: pid - %d, wartosc semafora - %d\n", i, pid, sem_value);

		if((fd = open("numer.txt", O_RDONLY)) == error) {	/* obsluga bledow open() */
			printf("	Numer bledu: %d\n", errno);
			perror("	open error");
			exit(EXIT_FAILURE);
		}

		if(read(fd, buffer, size) == error) {			/* obsluga bledow read() */
			printf("	Numer bledu: %d\n", errno);
			perror("	read error");
			exit(EXIT_FAILURE);
		}
		numer = atoi(buffer);
		printf("	Odczytano z pliku numer: %d\n", numer);
		memset(buffer, 0, sizeof(buffer));			/* czyszczenie bufora */

		if(close(fd) == error) {				/* zamkniecie deskryptora */
			printf("	Numer bledu: %d\n", errno);	/* obsluga bledow close() */
			perror("	close error");
			exit(EXIT_FAILURE);
		}

		numer += 1;

		usleep(rand() % (max + 1 - min) + min);	/* symulowanie roznych szybkosci dzialania programu */

		snprintf_return = snprintf(buffer, size, "%d", numer);

		if((fd = open("numer.txt", O_WRONLY | O_TRUNC)) == error) {	/* obsluga bledow open() */
			printf("	Numer bledu: %d\n", errno);
			perror("	open error");
			exit(EXIT_FAILURE);
		}

		if(write(fd, buffer, snprintf_return) == error) {	/* obsluga bledow write() */
			printf("	Numer bledu: %d\n", errno);
			perror("	write error");
			exit(EXIT_FAILURE);
		}
		memset(buffer, 0, sizeof(buffer));			/* czyszczenie bufora */

		if(close(fd) == error) {				/* zamkniecie deskryptora */
			printf("	Numer bledu: %d\n", errno);	/* obsluga bledow close() */
			perror("	close error");
			exit(EXIT_FAILURE);
		}

		semV(sem_address);

		sem_value += 1;

		printf("Po sesji krytycznej numer %d: pid - %d, wartosc semafora - %d\n", i, pid, sem_value);
	}

	semClose(sem_address);

	return EXIT_SUCCESS;
}