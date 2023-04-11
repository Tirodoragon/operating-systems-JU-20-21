#define _DEFAULT_SOURCE

#include "sem.h"
#include "shmem.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

enum {
	error = -1
};

void pisz(int fd, cykliczny *ptr, int size_end) {
	int write_return;
	if((write_return = write(fd, ptr->buf[ptr->beg], size_end)) == error) {	/* pobieranie towaru z bufora cyklicznego i umieszczanie go w pliku */
		printf("Numer bledu: %d\n", errno);				/* obsluga bledow write() */
		perror("write error");
		exit(EXIT_FAILURE);
	}
}

void zapis(semid sem_address_pro, semid sem_address_con, int fd, cykliczny *ptr) {
	int size = sizeof(ptr->buf[ptr->beg]);
	enum {
		byte = 1
	};
	int write_return;
	srand(time(NULL));											/* seed z czasu biezacego, aby przy kazdym uruchomieniu programu byly rozne wartosci rand() */
	int min = 1;													/* przedzial generowanych wartosci przez rand() */
	int max = 1000000;						/* 1 zeby sleep trwal minimum jedna mikrosekunde, a 1000000 poniewaz powyzej tej liczby w niektorych systemach jest to blad w usleep() */
	while(1) {
		semP(sem_address_con);	/* opuszczenie semafora konsumenta */

		if(ptr->buf[ptr->beg][ptr->koniec] == '\0') {								/* obsluga dojscia do konca bufora cyklicznego */
			printf("Konsument pobral towar z bufora cyklicznego i umiescil go w schowku. Pobrany towar: ");	/* wypisanie na ekran danych przesylanych przez konsumenta */
			fflush(stdout);											/* aby powyzszy printf wypisal sie od razu */
			printf("%.*s\n", ptr->koniec, ptr->buf[ptr->beg]);

			pisz(fd, ptr, ptr->koniec);	/* pobieranie towaru z bufora cyklicznego i umieszczanie go w pliku */

			return;
		}

		printf("Konsument pobral towar z bufora cyklicznego i umiescil go w schowku. Pobrany towar: ");	/* wypisanie na ekran danych przesylanych przez konsumenta */
		fflush(stdout);											/* aby powyzszy printf wypisal sie od razu */
		printf("%.*s\n", NELE, ptr->buf[ptr->beg]);

		if((write_return = write(fd, ptr->buf[ptr->beg], size)) == error) {	/* pobieranie towaru z bufora cyklicznego i umieszczanie go w pliku */
			printf("Numer bledu: %d\n", errno);				/* obsluga bledow write() */
			perror("write error");
			exit(EXIT_FAILURE);
		}

		ptr->beg = (ptr->beg + 1) % ptr->size;	/* przesuniecie pozycji wstawiania o 1 dalej */

		semV(sem_address_pro);	/* podniesienie semafora producenta */

		usleep(rand() % (max + 1 - min) + min);	/* symulowanie roznych predkosci dzialania programu */
	}
}

int main(int argc, char* argv[]) {
	enum {
		file_access_permissions = 0644
	};
	int fd[2];										/* tablica deskryptorow plikow */
	if((fd[0] = open(argv[1], O_WRONLY | O_CREAT, file_access_permissions)) == error) {	/* otwarcie pliku, ktorego nazwa znajduje sie w argv[1], w ktorym bedziemy umieszczac towar */
		printf("Numer bledu: %d\n", errno);						/* obsluga bledow open() */
		perror("open error");
		exit(EXIT_FAILURE);
	}

	enum {
		sem_slash_max = 251		/* maksymalna ilosc znakow w nazwie semafora z ukosnikiem */
	};
	char sem_slash_name_pro[sem_slash_max];	/* nazwa semafora do wstrzymywania producenta z ukosnikiem */
	char sem_slash_name_con[sem_slash_max];	/* nazwa semafora do wstrzymywania konsumenta z ukosnikiem */

	strcpy(sem_slash_name_pro, argv[2]);
	strcpy(sem_slash_name_con, argv[3]);

	semid sem_address_pro = semOpen(sem_slash_name_pro);
	semid sem_address_con = semOpen(sem_slash_name_con);

	enum {
		shm_slash_max = 255		/* maksymalna ilosc znakow w nazwie obiektu pamieci dzielonej z ukosnikiem */
	};
	char shm_slash_name[shm_slash_max];	/* nazwa obiektu pamieci dzielonej z ukosnikiem */

	strcpy(shm_slash_name, argv[4]);

	fd[1] = shmOpen(shm_slash_name);

	int size = sizeof(cykliczny);
	cykliczny *ptr;			/* adres poczatku odwzorowanego obszaru pamieci dzielonej */
	ptr = (cykliczny *) shmMap(fd[1], size);

	zapis(sem_address_pro, sem_address_con, fd[0], ptr);

	if(close(fd[0]) == error) {			/* zamkniecie pliku, w ktorym umieszczono towar */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close error");
		exit(EXIT_FAILURE);
	}

	semClose(sem_address_pro);
	semClose(sem_address_con);

	shmClose(ptr, fd[1], size);

	return EXIT_SUCCESS;
}