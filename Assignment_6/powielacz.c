#include "sem.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

enum {
	name_max = 250		/* maksymalna ilosc znakow w nazwie semafora bez ukosnika */
};
enum {
	slash_max = 251		/* maksymalna ilosc znakow w nazwie semafora z ukosnikiem */
};
char sem_name[name_max];	/* nazwa semafora */
char slash_name[slash_max];	/* napis, ktory rozpoczyna sie ukosnikiem i do ktorego zostanie dolaczona nazwa semafora */

void my_sighandler(int numer_sygnalu) {	/* obsluga sygnalu SIGINT, ktora bedzie usuwac semafor - na wypadek gdyby program trzeba bylo zakonczyc sekwencja klawiszy Ctrl+C */
	semRemove(slash_name);
}

void koniec(void) {		/* funkcja wywolywana przy zakonczeniu procesu */
	semRemove(slash_name);
}

int main(int argc, char* argv[]) {
	enum {
		error = -1
	};

	enum {
		liczba_argumentow = 4
	};
	if(argc != liczba_argumentow) {														/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu %s ./prog.x liczba_procesow liczba_sekcji_krytycznych\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd;											/* deskryptor pliku */
	enum {
		file_access_permissions = 0644
	};
	if((fd = open("numer.txt", O_WRONLY | O_CREAT, file_access_permissions)) == error) {	/* otwarcie pliku, w celu weryfikacji poprawnosci wzajemnego wykluczania procesow */
		printf("Numer bledu: %d\n", errno);						/* obsluga bledow open() */
		perror("open error");
		exit(EXIT_FAILURE);
	}

	enum {
		size = 12
	};
	char buffer[size];
	enum {
		numer_poczatkowy = 0
	};
	int snprintf_return;
	snprintf_return = snprintf(buffer, size, "%d", numer_poczatkowy);	/* wpisanie do pliku kontrolnego wartosci zero */
	if(write(fd, buffer, snprintf_return) == error) {			/* obsluga bledow write() */
		printf("Numer bledu: %d\n", errno);
		perror("write error");
		exit(EXIT_FAILURE);
	}
	memset(buffer, 0, sizeof(buffer));					/* czyszczenie bufora */

	if(close(fd) == error) {			/* zamkniecie deskryptora */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close error");
		exit(EXIT_FAILURE);
	}

	printf("Prosze podac nazwe tworzonego semafora (maksymalnie 249 znakow): ");
	scanf("%249s", sem_name);

	slash_name[0] = '/';
	strcat(slash_name, sem_name);	/* dolaczenie do nazwy semafora ukosnika na poczatku */

	enum {
		sem_value = 1	/* wartosc poczatkowa semafora */
	};

	semid sem_address = semCreate(slash_name, sem_value);

	if(signal(2, my_sighandler) == SIG_ERR) {		/* ustawienie obslugi sygnalu SIGINT */
		printf("Numer bledu: %d\n", errno);		/* obsluga bledow signal() */
		perror("Funkcja signal ma problem z SIGINT");
		exit(EXIT_FAILURE);
	}

	enum {
		atexit_error = 0
	};
	if(atexit(koniec) != atexit_error) {		/* zmodyfikowanie zakonczenia procesu, aby usunieto wtedy semafor */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow atexit() */
		perror("atexit error");
		exit(EXIT_FAILURE);
	}

	int liczba_procesow = atoi(argv[2]);
	int liczba_sekcji_krytycznych = atoi(argv[3]);
	snprintf(buffer, size, "%d", sem_value);
	char *arg1 = buffer;
	char *arg2 = slash_name;
	for(int i = 0; i < liczba_procesow; i++) {								/* powielanie procesow realizujacych wzajemne wykluczanie */
		switch(fork()) {
			case -1:					/* obsluga bledow fork() */
				printf("Numer bledu: %d\n", errno);
				perror("fork error");
				exit(EXIT_FAILURE);

				break;

			case 0:											/* proces potomny - realizujacy wzajemne wykluczanie */
				if(execl(argv[1], argv[1], argv[3], arg1, arg2, (char *) NULL) == error) {	/* obsluga bledow execl() */
					printf("Numer bledu: %d\n", errno);
					perror("execl error");
					_exit(EXIT_FAILURE);
				}

				break;

			default:	/* proces macierzysty */
				break;
		}
	}
	memset(buffer, 0, sizeof(buffer));									/* czyszczenie bufora */

	for(int j = 0; j < liczba_procesow; j++) {		/* czekanie na procesy potomne */
		if(wait(NULL) == error) {			/* obsluga bledow wait() */
			printf("Numer bledu: %d\n", errno);
			perror("wait error");
			_exit(EXIT_FAILURE);
		}
	}

	if((fd = open("numer.txt", O_RDONLY)) == error) {	/* obsluga bledow open() */
		printf("Numer bledu: %d\n", errno);
		perror("open error");
		exit(EXIT_FAILURE);
	}

	if(read(fd, buffer, size) == error) {									/* sprawdzenie poprawnosci koncowego numeru zapisanego w pliku */
		printf("Numer bledu: %d\n", errno);
		perror("read error");
		exit(EXIT_FAILURE);
	}
	int koncowa_wartosc = atoi(buffer);
	printf("Koncowa wartosc: %d\n", koncowa_wartosc);
	int spodziewana_wartosc = liczba_procesow * liczba_sekcji_krytycznych;
	if(koncowa_wartosc == spodziewana_wartosc) {
		printf("Poprawna synchronizacja procesow. Wartosc spodziewana: %d\n", spodziewana_wartosc);
	}
	else {
		printf("Niepoprawna synchronizacja procesow. Wartosc spodziewana: %d\n", spodziewana_wartosc);
	}
	memset(buffer, 0, sizeof(buffer));									/* czyszczenie bufora */

	if(close(fd) == error) {			/* zamkniecie deskryptora */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close */
		perror("close error");
		exit(EXIT_FAILURE);
	}

	semClose(sem_address);

	return EXIT_SUCCESS;
}