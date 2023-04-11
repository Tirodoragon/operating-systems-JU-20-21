#define _DEFAULT_SOURCE

#include "piekarnia.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define LICZBA_SEKCJI_KRYTYCZNYCH 1000000

volatile int licznik = 0;	/* globalny licznik, volatile aby zapobiec optymalizacji przez kompilator */

void *wykluczanie(void *arg) {
	int id = *(int *) arg;
	free(arg);
	for(int k = 0; k < LICZBA_SEKCJI_KRYTYCZNYCH; k++) {
		lock(id);	/* wejscie do sekcji krytycznej wedlug algorytmu piekarni */

		licznik = licznik + 1;

		unlock(id);	/* wyjscie z sekcji krytycznej wedlug algorytmu piekarni */
	}

	printf("Watek %d wartosc koncowa %d\n", id, licznik);

	pthread_exit(NULL);
}

int main() {
	enum {
		error = 0
	};
	enum {
		buf_max = 50
	};
	char buf[buf_max];

	memset((void *) numer, 0, sizeof(numer));
	memset((void *) wybieranie, 0, sizeof(wybieranie));

	pthread_t watki[LICZBA_WATKOW];

	int pthread_create_return;
	for(int i = 0; i < LICZBA_WATKOW; i++) {
		int *arg = malloc(sizeof(int));
		*arg = i;
		if((pthread_create_return = pthread_create(&watki[i], NULL, wykluczanie, arg)) != error) {	/* obsluga bledow pthread_create() */
			printf("Numer bledu: %d\n", pthread_create_return);
			strerror_r(pthread_create_return, buf, buf_max);
			printf("pthread_create error: %s\n", buf);
			exit(EXIT_FAILURE);
		}
	}

	int pthread_join_return;
	for(int j = 0; j < LICZBA_WATKOW; j++) {
		if((pthread_join_return = pthread_join(watki[j], NULL)) != error) {	/* obsluga bledow pthread_join() */
			printf("Numer bledu: %d\n", pthread_join_return);
			strerror_r(pthread_join_return, buf, buf_max);
			printf("pthread_join error: %s\n", buf);
			exit(EXIT_FAILURE);
		}
	}

	int spodziewana_wartosc = LICZBA_WATKOW * LICZBA_SEKCJI_KRYTYCZNYCH;
	printf("Koncowa wartosc: %d\n", licznik);
	if(licznik == spodziewana_wartosc) {
		printf("Poprawna synchronizacja watkow. Wartosc spodziewana: %d\n", spodziewana_wartosc);
	}
	else {
		printf("Niepoprawna synchronizacja watkow. Wartosc spodziewana: %d\n", spodziewana_wartosc);
	}

	return EXIT_SUCCESS;
}