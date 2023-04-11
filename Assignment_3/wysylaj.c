#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	if(argc != 3) {													/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu: %s rodzaj_akcji numer_sygnalu\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pid_t pid;
	int numer_sygnalu = atoi(argv[2]);

	switch(pid = fork()) {
		case -1:					/* obsluga bledow fork() */
			printf("Numer bledu: %d\n", errno);
			perror("fork error");
			exit(EXIT_FAILURE);
			break;

		case 0:												/* proces potomny */
			if(execl("./obsluga.x", "./obsluga.x", argv[1], argv[2], (char *) NULL) == -1) {	/* obsluga bledow execl() */
				printf("Numer bledu: %d\n", errno);
				perror("execl error");
				_exit(EXIT_FAILURE);
			}
			break;

		default: 						/* proces macierzysty */
			sleep(1);
			kill(pid, 0);					/* sprawdzenie, czy proces istnieje */
			if(errno == ESRCH) {
				printf("Numer bledu: %d\n", errno);
				perror("kill error");
				exit(EXIT_FAILURE);
			}
			if(kill(pid, numer_sygnalu) == -1) {		/* obsluga bledow kill() */
				printf("Numer bledu: %d\n", errno);
				perror("kill error");
				exit(EXIT_FAILURE);
			}
			if(wait(NULL) == -1) {				/* obsluga bledow wait() */
				printf("Numer bledu: %d\n", errno);
				perror("wait error");
				exit(EXIT_FAILURE);
			}
			break;
	}

	return 0;
}