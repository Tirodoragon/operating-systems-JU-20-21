#include "sem.h"
#include "shmem.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

enum {
	sem_name_max = 250		/* maksymalna ilosc znakow w nazwie semafora bez ukosnika */
};
enum {
	sem_slash_max = 251		/* maksymalna ilosc znakow w nazwie semafora z ukosnikiem */
};
char sem_name_pro[sem_name_max];	/* nazwa semafora do wstrzymywania producenta */
char sem_slash_name_pro[sem_slash_max];	/* napis, ktory rozpoczyna sie ukosnikiem i do ktorego zostanie dolaczona nazwa semafora do wstrzymywania producenta */
char sem_name_con[sem_name_max];	/* nazwa semafora do wstrzymywania konsumenta */
char sem_slash_name_con[sem_slash_max];	/* napis, ktory rozpoczyna sie ukosnikiem i do ktorego zostanie dolaczona nazwa semafora do wstrzymywania konsumenta */

enum {
	shm_name_max = 254		/* maksymalna ilosc znakow w nazwie obiektu pamieci dzielonej bez ukosnika */
};
enum {
	shm_slash_max = 255		/* maksymalna ilosc znakow w nazwie obiektu pamieci dzielonej z ukosnikiem */
};
char shm_name[shm_name_max];		/* nazwa obiektu pamieci dzielonej */
char shm_slash_name[shm_slash_max];	/* napis, ktory rozpoczyna sie ukosnikiem i do ktorego zostanie dolaczona obiektu pamieci dzielonej */

void my_sighandler(int numer_sygnalu) {	/* obsluga sygnalu SIGINT, ktora bedzie usuwac semafor i obiekt pamieci dzielonej - na wypadek gdyby program trzeba bylo zakonczyc sekwencja klawiszy Ctrl+C */
	semRemove(sem_slash_name_pro);
	semRemove(sem_slash_name_con);
	shmRm(shm_slash_name);
}

void koniec(void) {			/* funkcja wywolywana przy zakonczeniu procesu */
	semRemove(sem_slash_name_pro);
	semRemove(sem_slash_name_con);
	shmRm(shm_slash_name);
}

int main(int argc, char* argv[]) {
	enum {
		error = -1
	};

	enum {
		liczba_argumentow = 5
	};
	if(argc != liczba_argumentow) {														/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu %s producent.x konsument.x magazyn.txt schowek.txt\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	printf("Prosze podac nazwe tworzonego semafora do wstrzymywania producenta (maksymalnie 249 znakow): ");
	scanf("%249s", sem_name_pro);

	sem_slash_name_pro[0] = '/';
	strcat(sem_slash_name_pro, sem_name_pro);	/* dolaczenie do nazwy semafora ukosnika na poczatku */

	enum {
		sem_value_pro = NBUF	/* wartosc poczatkowa semafora do wstrzymywania producenta */
	};

	semid sem_address_pro = semCreate(sem_slash_name_pro, sem_value_pro);

	printf("Prosze podac nazwe tworzonego semafora do wstrzymywania konsumenta (maksymalnie 249 znakow): ");
	scanf("%249s", sem_name_con);

	sem_slash_name_con[0] = '/';
	strcat(sem_slash_name_con, sem_name_con);	/* dolaczenie do nazwy semafora ukosnika na poczatku */

	enum {
		sem_value_con = 0	/* wartosc poczatkowa semafora do wstrzymywania konsumenta */
	};

	semid sem_address_con = semCreate(sem_slash_name_con, sem_value_con);

	printf("Prosze podac nazwe tworzonej pamieci dzielonej (maksymalnie 253 znakow): ");
	scanf("%253s", shm_name);

	shm_slash_name[0] = '/';
	strcat(shm_slash_name, shm_name);

	int size = sizeof(cykliczny);
	int fd;					/* deskryptor obiektu pamieci dzielonej */
	fd = shmCreate(shm_slash_name, size);

	cykliczny *ptr;				/* adres poczatku odwzorowanego obszaru pamieci dzielonej */
	ptr = (cykliczny *) shmMap(fd, size);

	if(signal(2, my_sighandler) == SIG_ERR) {		/* ustawienie obslugi sygnalu SIGINT */
		printf("Numer bledu: %d\n", errno);		/* obsluga bledow signal() */
		perror("Funkcja signal ma problem z SIGINT");
		exit(EXIT_FAILURE);
	}

	enum {
		atexit_error = 0
	};
	if(atexit(koniec) != atexit_error) {		/* zmodyfikowanie zakonczenia procesu, aby usunieto wtedy semafory i obiekt pamieci dzielonej */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow atexit() */
		perror("atexit error");
		exit(EXIT_FAILURE);
	}

	ptr->size = NBUF;
	ptr->end = 0;
	ptr->beg = 0;
	ptr->koniec = 0;

	char *arg1 = sem_slash_name_pro;
	char *arg2 = sem_slash_name_con;
	char *arg3 = shm_slash_name;
	switch(fork()) {											/* producent */
		case -1:					/* obsluga bledow fork() */
			printf("Numer bledu: %d\n", errno);
			perror("fork error");
			exit(EXIT_FAILURE);

			break;

		case 0:												/* proces potomny */
			if(execl(argv[1], argv[1], argv[3], arg1, arg2, arg3, (char *) NULL) == error) {	/* obsluga bledow execl() */
				printf("Numer bledu: %d\n", errno);
				perror("execl error");
				_exit(EXIT_FAILURE);
			}

			break;

		default:	/* proces macierzysty */
			break;
	}

	switch(fork())	{									/* konsument */
		case -1:					/* obsluga bledow fork() */
			printf("Numer bledu: %d\n", errno);
			perror("fork error");
			exit(EXIT_FAILURE);

			break;

		case 0:										/* proces potomny */
			if(execl(argv[2], argv[2], argv[4], arg1, arg2, arg3, (char *) NULL) == error) {	/* obsluga bledow execl() */
				printf("Numer bledu: %d\n", errno);
				perror("execl error");
				_exit(EXIT_FAILURE);
			}

			break;

		default:	/* proces macierzysty */
			break;
	}

	for(int j = 0; j < 2; j++) {				/* czekanie na procesy potomne */
		if(wait(NULL) == error) {			/* obsluga bledow wait() */
			printf("Numer bledu: %d\n", errno);
			perror("wait error");
			_exit(EXIT_FAILURE);
		}
	}

	semClose(sem_address_pro);
	semClose(sem_address_con);

	shmClose(ptr, fd, size);

	return EXIT_SUCCESS;
}