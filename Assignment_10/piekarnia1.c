#define _DEFAULT_SOURCE

#include "piekarnia.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define LICZBA_SEKCJI_KRYTYCZNYCH 7

int cols;

enum {
	x = 1
};
enum {
	y = 1
};

int globalny_licznik = 0;

void *wykluczanie(void *arg) {
	int id = *(int *) arg;
	free(arg);
	printf("\033[%d;%dH\033[1m\033[32mWatek %d - numer %d\n", y + id, x, id, globalny_licznik);
	enum {
		margin = 20
	};
	int prywatny_licznik;
	srand(time(NULL));											/* seed z czasu biezacego, aby przy kazdym uruchomieniu programu byly rozne wartosci rand() */
	int min = 1;														/* przedzial generowanych wartosci przez rand() */
	int max = 1000000;						/* 1 zeby sleep trwal minimum jedna mikrosekunde, a 1000000 poniewaz powyzej tej liczby w niektorych systemach jest to blad w usleep() */
	for(int k = 0; k < LICZBA_SEKCJI_KRYTYCZNYCH; k++) {
		lock(id);	/* wejscie do sekcji krytycznej wedlug algorytmu piekarni */

		printf("\033[%d;%dH\033[2K\033[31mWatek - %d - numer %d\n", y + id, cols - margin, id, globalny_licznik);

		prywatny_licznik = globalny_licznik;

		prywatny_licznik++;

		usleep(rand() % (max + 1 - min) + min);	/* symulowanie roznych szybkosci dzialania programu */

		globalny_licznik = prywatny_licznik;

		unlock(id);	/* wyjscie z sekcji krytycznej wedlug algorytmu piekarni */

		printf("\033[%d;%dH\033[2K\033[32mWatek %d - numer %d\n", y + id, x, id, globalny_licznik);
	}

	pthread_exit(NULL);
}

int main() {
	enum {
		error_0 = 0
	};
	enum {
		buf_max = 50
	};
	char buf[buf_max];
	enum {
		error_1 = -1
	};

	enum {
		komenda_max = 10
	};
	char komenda[komenda_max];
	strcpy(komenda, "tput cols");
	enum {
		typ_max = 2
	};
	char typ[typ_max];
	strcpy(typ, "r");
	FILE *fp;
	if((fp = popen(komenda, typ)) == NULL) {	/* obsluga bledow popen() */
		printf("Numer bledu: %d\n", errno);
		perror("popen error");
		exit(EXIT_FAILURE);
	}
	enum {
		komenda_r_max = 4
	};
	char komenda_r[komenda_r_max];
	if(fgets(komenda_r, komenda_r_max, fp) == NULL) {	/* obsluga bledow fgets() */
		printf("Blad w funkcji fgets!\n");
		exit(EXIT_FAILURE);
	}
	cols = atoi(komenda_r);

	printf("\033c");

	memset((void *) numer, 0, sizeof(numer));
	memset((void *) wybieranie, 0, sizeof(wybieranie));

	pthread_t watki[LICZBA_WATKOW];

	int pthread_create_return;
	for(int i = 0; i < LICZBA_WATKOW; i++) {
		int *arg = malloc(sizeof(int));
		*arg = i;
		if((pthread_create_return = pthread_create(&watki[i], NULL, wykluczanie, arg)) != error_0) {	/* obsluga bledow pthread_create() */
			printf("Numer bledu: %d\n", pthread_create_return);
			strerror_r(pthread_create_return, buf, buf_max);
			printf("pthread_create error: %s\n", buf);
			exit(EXIT_FAILURE);
		}
	}

	int pthread_join_return;
	for(int j = 0; j < LICZBA_WATKOW; j++) {
		if((pthread_join_return = pthread_join(watki[j], NULL)) != error_0) {	/* obsluga bledow pthread_join() */
			printf("Numer bledu: %d\n", pthread_join_return);
			strerror_r(pthread_join_return, buf, buf_max);
			printf("pthread_join error: %s\n", buf);
			exit(EXIT_FAILURE);
		}
	}

	printf("\033[%d;%dH", y + LICZBA_WATKOW, x);
	int spodziewana_wartosc = LICZBA_WATKOW * LICZBA_SEKCJI_KRYTYCZNYCH;
	printf("Koncowa wartosc: %d\n", globalny_licznik);
	if(globalny_licznik == spodziewana_wartosc) {
		printf("Poprawna synchronizacja watkow. Wartosc spodziewana: %d\n", spodziewana_wartosc);
	}
	else {
		printf("\033[31m");
		printf("Niepoprawna synchronizacja watkow. Wartosc spodziewana: %d\n", spodziewana_wartosc);
	}

	int pclose_return;
	if((pclose_return = pclose(fp)) == error_1) {	/* obsluga bledow pclose() */
		printf("Numer bledu: %d\n", errno);
		perror("pclose error");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}