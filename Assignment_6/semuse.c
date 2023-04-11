#include "sem.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
	enum {
		liczba_argumentow = 2
	};
	if(argc != liczba_argumentow) {									/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu: %s operacja\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int operacja = *argv[1];
	if(operacja != 'i' && operacja != 'r' && operacja != 'p' && operacja != 'v' && operacja != 'q') {	/* obsluga blednego argumentu */
		printf("Zly argument. Prawidlowa wartosc: i, r, p, v lub q\n");
		exit(EXIT_FAILURE);
	}

	enum {
		name_max = 250														/* maksymalna ilosc znakow w nazwie semafora bez ukosnika */
	};
	enum {
		slash_max = 251														/* maksymalna ilosc znakow w nazwie semafora z ukosnikiem */
	};
	char sem_name[name_max];													/* nazwa semafora */
	char slash_name[slash_max];											/* napis, ktory rozpoczyna sie ukosnikiem i do ktorego zostanie dolaczona nazwa semafora */
	slash_name[0] = '/';
	semid sem_address;
	switch(operacja) {
		case 'i':
			printf("Prosze podac nazwe tworzonego semafora (maksymalnie 249 znakow): ");
			scanf("%249s", sem_name);

			strcat(slash_name, sem_name);	/* dolaczenie do nazwy semafora ukosnika na poczatku */

			int sem_value;
			printf("Prosze podac wartosc poczatkowa semafora: ");
			scanf("%d", &sem_value);

			sem_address = semCreate(slash_name, sem_value);

			semInfo(sem_address);

			semClose(sem_address);

			break;

		case 'r':
			printf("Prosze podac nazwe semafora do usuniecia (maksymalnie 249 znakow): ");
			scanf("%249s", sem_name);

			strcat(slash_name, sem_name);	/* dolaczenie do nazwy semafora ukosnika na poczatku */

			semRemove(slash_name);

			break;

		case 'p':
			printf("Prosze podac nazwe semafora do opuszczenia (maksymalnie 249 znakow): ");
			scanf("%249s", sem_name);

			strcat(slash_name, sem_name);	/* dolaczenie do nazwy semafora ukosnika na poczatku */

			sem_address = semOpen(slash_name);

			semP(sem_address);

			semInfo(sem_address);

			semClose(sem_address);

			break;

		case 'v':
			printf("Prosze podac nazwe semafora do podniesienia (maksymalnie 249 znakow): ");
			scanf("%249s", sem_name);

			strcat(slash_name, sem_name);	/* dolaczenie do nazwy semafora ukosnika na pcozatku */

			sem_address = semOpen(slash_name);

			semV(sem_address);

			sleep(1);	/* aby wypisywac poprawna wartosc semafora */

			semInfo(sem_address);

			semClose(sem_address);

			break;

		case 'q':
			printf("Prosze podac nazwe semafora, o ktorym maja zostac wypisane informacje (maksymalnie 249 znakow): ");
			scanf("%249s", sem_name);

			strcat(slash_name, sem_name);	/* dolaczenie do nazwy semafora ukosnika na poczatku */

			sem_address = semOpen(slash_name);

			semInfo(sem_address);

			semClose(sem_address);

			break;

		default:
			break;
	}

	return EXIT_SUCCESS;
}