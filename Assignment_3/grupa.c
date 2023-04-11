#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


void forking(char* rodzaj_akcji, char* numer_sygnalu) {
	switch(fork()){
		case -1:					/* obsluga bledow fork() */
			printf("Numer bledu: %d\n", errno);
			perror("fork error");
			exit(EXIT_FAILURE);
			break;

		case 0:
			if(execl("./obsluga.x", "./obsluga.x", rodzaj_akcji, numer_sygnalu, (char *) NULL) == -1) {	/* obsluga bledow execl() */
				printf("Numer bledu: %d\n", errno);
				perror("execl error");
				_exit(EXIT_FAILURE);
			}
		default:
			break;
	}
}

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


		case 0:											/* proces potomny */
			if(setpgid(0, 0) == -1) {							/* aby proces potomny stal sie liderem nowej grupy procesow */
				printf("Numer bledu: %d\n", errno);					/* obsluga bledow setpgid() */
				perror("setpgid error");
				_exit(EXIT_FAILURE);
			}
			forking(argv[1], argv[2]);
			forking(argv[1], argv[2]);
			forking(argv[1], argv[2]);
			if(signal(numer_sygnalu, SIG_IGN) == SIG_ERR) {					/* obsluga bledow signal() */
				printf("Numer bledu: %d\n", errno);
				perror("execl error");
				_exit(EXIT_FAILURE);
			}
			int pid1 = wait(NULL);
			if(pid1 == -1) {								/* obsluga bledow wait() */
				printf("Numer bledu: %d\n", errno);
				perror("wait error");
				_exit(EXIT_FAILURE);
			}
			int pid2 = wait(NULL);
			if(pid2 == -1) {								/* obsluga bledow wait() */
				printf("Numer bledu: %d\n", errno);
				perror("wait error");
				_exit(EXIT_FAILURE);
			}
			int pid3 = wait(NULL);
			if(pid3 == -1) {								/* obsluga bledow wait() */
				printf("Numer bledu: %d\n", errno);
				perror("wait error");
				_exit(EXIT_FAILURE);
			}
			printf("Pomyslnie zakonczono proces potomny o PID = %d, PID = %d, PID = %d\n", pid1, pid2, pid3);
			break;

		default:						/* proces macierzysty */
			sleep(1);
			if(getpgid(pid) == -1) {			/* obsluga bledow getpgid() */
				printf("Numer bledu: %d\n", errno);
				perror("getpgid error");
				exit(EXIT_FAILURE);
			}
			kill(-getpgid(pid), 0);				/* sprawdzenie czy grupa procesow istnieje */
			if(errno == ESRCH) {
				printf("Numer bledu: %d\n", errno);
				perror("kill error");
				exit(EXIT_FAILURE);
			}
			if(kill(-getpgid(pid), numer_sygnalu) == -1) {	/* obsluga bledow kill() */
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