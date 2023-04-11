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

void odczyt(semid sem_address_pro, semid sem_address_con, int fd, cykliczny *ptr) {
	int size = sizeof(ptr->buf[ptr->end]);
	int read_return;
	srand(time(NULL));											/* seed z czasu biezacego, aby przy kazdym uruchomieniu programu byly rozne wartosci rand() */
	int min = 1;															/* przedzial generowanych wartosci przez rand() */
	int max = 1000000;						/* 1 zeby sleep trwal minimum jedna mikrosekunde, a 1000000 poniewaz powyzej tej liczby w niektorych systemach jest to blad w usleep() */
	while(1) {
		semP(sem_address_pro);	/* opuszczenie semafora producenta */

		if((read_return = read(fd, ptr->buf[ptr->end], size)) == error) {	/* pobieranie surowca z pliku i wstawianie go jako towar do bufora cyklicznego*/
			printf("Numer bledu: %d\n", errno);				/* obsluga bledow read() */
			perror("read error");
			exit(EXIT_FAILURE);
		}

		printf("Producent pobral surowiec z magazynu i wstawil go jako towar do bufora cyklicznego. Pobrany surowiec: ");	/* wypisanie na ekran danych przesylanych przez producenta */
		fflush(stdout);														/* aby powyzszy printf wypisal sie od razu */
		printf("%.*s\n", read_return, ptr->buf[ptr->end]);

		if(read_return < NELE) {			/* obsluga dojscia do konca pliku */
			ptr->buf[ptr->end][read_return] = '\0';	/* zapisanie na koniec pobierania danych znaku \0 */

			ptr->koniec = read_return;	/* wpisanie do struktury ilosci zapisanych ostatnich danych */

			semV(sem_address_con);	/* podniesienie semafora konsumenta */

			return;
		}

		ptr->end = (ptr->end + 1) % ptr->size;	/* przesuniecie pozycji wstawiania o 1 dalej */

		printf("Obecny rozmiar bufora cyklicznego: %d\n", (ptr->end - ptr->beg + ptr->size) % (ptr->size + 1));

		semV(sem_address_con);	/* podniesienie semafora konsumenta */

		usleep(rand() % (max + 1 - min) + min);	/* symulowanie roznych szybkosci dzialania programu */
	}
}

int main(int argc, char* argv[]) {
	int fd[2];						/* tablica deskryptorow plikow */
	if((fd[0] = open(argv[1], O_RDONLY)) == error) {	/* otwarcie pliku, ktorego nazwa znajduje sie w argv[1], z ktorego bedziemy pobierac surowiec */
		printf("Numer bledu: %d\n", errno);		/* obsluga bledow open() */
		perror("open error");
		exit(EXIT_FAILURE);
	}

	enum {
		sem_slash_max = 251			/* maksymalna ilosc znakow w nazwie semafora z ukosnikiem */
	};
	char sem_slash_name_pro[sem_slash_max];	/* nazwa semafora do wstrzymywania producenta z ukosnikiem */
	char sem_slash_name_con[sem_slash_max];	/* nazwa semafora do wstrzymywania konsumenta z ukosnikiem */

	strcpy(sem_slash_name_pro, argv[2]);
	strcpy(sem_slash_name_con, argv[3]);

	semid sem_address_pro = semOpen(sem_slash_name_pro);
	semid sem_address_con = semOpen(sem_slash_name_con);

	enum {
		shm_slash_max = 255			/* maksymalna ilosc znakow w nazwie obiektu pamieci dzielonej z ukosnikiem */
	};
	char shm_slash_name[shm_slash_max];	/* nazwa obiektu pamieci dzielonej z ukosnikiem */

	strcpy(shm_slash_name, argv[4]);

	fd[1] = shmOpen(shm_slash_name);

	int size = sizeof(cykliczny);
	cykliczny *ptr;					/* adres poczatku odwzorowanego obiektu pamieci dzielonej */
	ptr = (cykliczny *) shmMap(fd[1], size);

	odczyt(sem_address_pro, sem_address_con, fd[0], ptr);

	if(close(fd[0]) == error) {			/* zamkniecie pliku, z ktorego pobrano surowiec */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close error");
		exit(EXIT_FAILURE);
	}

	semClose(sem_address_pro);
	semClose(sem_address_con);

	shmClose(ptr, fd[1], size);

	return EXIT_SUCCESS;
}