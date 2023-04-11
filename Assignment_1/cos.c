#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	pid_t pid;
	pid_t sid;

	if(argc != 2) {												/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu: %s pid_procesu\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sid = getsid(0);
	if(sid == -1) {					/* obsluga bledow getsid() */
		printf("Numer bledu: %d\n", errno);
		perror("getsid error");
		exit(EXIT_FAILURE);
	}
	else {
		printf("SID: %d\n", sid);
	}

	pid = atoi(argv[1]);
	if(setpgid(pid, 1337) == -1) {			/* obsluga bledow setpgid() */
		printf("Numer bledu: %d\n", errno);
		perror("setpgid error");
		exit(EXIT_FAILURE);
	}

	return 0;
}