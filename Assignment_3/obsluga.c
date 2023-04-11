#include "procinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

extern const char * const sys_siglist[];

void my_sighandler(int numer_sygnalu) {				/* funkcja obslugujaca przechwycony sygnal */
	printf("Przechwycilem sygnal: %d\n", numer_sygnalu);
}

int main(int argc, char* argv[]) {
	if(argc != 3) {													/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu: %s rodzaj_akcji numer_sygnalu\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int rodzaj_akcji = *argv[1];
	if(rodzaj_akcji != 'd' && rodzaj_akcji != 'i' && rodzaj_akcji != 'p') {		/* obsluga blednego pierwszego argumentu */
		printf("Zly pierwszy argument. Prawidlowa wartosc: d, i lub p\n");
		exit(EXIT_FAILURE);
	}

	procinfo(argv[0]);

	int numer_sygnalu = atoi(argv[2]);

	char buffer[50];
	snprintf(buffer, sizeof (buffer), "Funkcja signal ma problem z %s", sys_siglist[numer_sygnalu]);

	switch(rodzaj_akcji) {
		case 'd':
			if(signal(numer_sygnalu, SIG_DFL) == SIG_ERR) {	/* obsluga bledow signal() */
				printf("Numer bledu: %d\n", errno);
				perror(buffer);
				exit(EXIT_FAILURE);
			}
			break;

		case 'i':
			if(signal(numer_sygnalu, SIG_IGN) == SIG_ERR) {	/* obsluga bledow signal() */
				printf("Numer bledu: %d\n", errno);
				perror(buffer);
				exit(EXIT_FAILURE);
			}
			break;

		case 'p':
			if(signal(numer_sygnalu, my_sighandler) == SIG_ERR) {	/* obsluga bledow signal() */
				printf("Numer bledu: %d\n", errno);
				perror(buffer);
				exit(EXIT_FAILURE);
			}
			break;

		default:
			break;
	}

	pause();	/* oczekiwanie na sygnal */

	return 0;
}