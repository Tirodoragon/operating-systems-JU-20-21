#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

void koniec(void) {					/* funkcja wywolywana przy zakonczeniu procesu */
	if(unlink("potok_nazwany") == -1) {		/* usuniecie potoku nazwanego */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow unlink() */
		perror("unlink error");
	}
}

void my_sighandler(int numer_sygnalu) {			/* obsluga sygnalu SIGINT, ktora bedzie usuwac potok - na wypadek gdyby program trzeba bylo zakonczyc sekwencja klawiszy Ctrl-C */
	if(unlink("potok_nazwany") == -1) {		/* usuniecie potoku nazwanego */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow unlink() */
		perror("unlink error");
		_exit(EXIT_FAILURE);
	}
}

int main(int argc, char* argv[]) {
	if(atexit(koniec) != 0) {			/* zmodyfikowanie zakoczenia procesu, aby usunieto wtedy potok nazwany */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow atexit() */
		perror("atexit error");
		exit(EXIT_FAILURE);
	}

	if(argc != 6) {																		/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu: %s potok_nazwany producent.x konsument.x magazyn.txt schowek.txt\n", argv[0]);
		_exit(EXIT_FAILURE);
	}

	if(signal(2, my_sighandler) == SIG_ERR) {		/* ustawienie obslugi sygnalu SIGINT */
		printf("Numer bledu: %d\n", errno);		/* obsluga bledow signal() */
		perror("Funkcja signal ma problem z SIGINT");
		_exit(EXIT_FAILURE);
	}

	if(mkfifo(argv[1], 0644) == -1) {		/* utworzenie potoku nazwanego z domyslnymi prawami dostepu */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow mkfifo() */
		perror("mkfifo error");
		_exit(EXIT_FAILURE);
	}

	switch(fork()) {					/* utworzenie procesu producenta */
		case -1:					/* obsluga bledow fork() */
			printf("Numer bledu: %d\n", errno);
			perror("fork error");
			exit(EXIT_FAILURE);
			break;

		case 0:										/* proces potomny - producent */
			if(execl(argv[2], argv[2], argv[1], argv[4], (char *) NULL) == -1) {	/* obsluga bledow execl() */
				printf("Numer bledu: %d\n", errno);
				perror("execl error");
				_exit(EXIT_FAILURE);
			}
			break;

		default:	/* proces macierzysty */
			break;
	}
	switch(fork()) {					/* utworzenie procesu konsumenta */
		case -1:					/* obsluga bledow fork() */
			printf("Numer bledu: %d\n", errno);
			perror("fork error");
			exit(EXIT_FAILURE);
			break;

		case 0:										/* proces potomny - konsument */
			if(execl(argv[3], argv[3], argv[1], argv[5],  (char *) NULL) == -1) {	/* obsluga bledow execl() */
				printf("Numer bledu: %d\n", errno);
				perror("execl error");
				_exit(EXIT_FAILURE);
			}
			break;

		default:	/* proces macierzysty */
			break;
	}

	if(wait(NULL) == -1) {				/* czekanie na pierwszy proces potomny */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow wait() */
		perror("wait error");
		_exit(EXIT_FAILURE);
	}
	if(wait(NULL) == -1) {				/* czekanie na drugi proces potomny */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow wait() */
		perror("wait error");
		_exit(EXIT_FAILURE);
	}

	return 0;
}