#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	pid_t pid = fork();	/* wartosc zwracana przez fork() */

	switch(pid) {
		case -1:					/* obsluga bledow fork() */
			printf("Numer bledu: %d\n", errno);
			perror("fork error");
			exit(EXIT_FAILURE);
			break;

		case 0:			/* proces potomny */
			sleep(60);	/* aby proces potomny stal sie procesem sierota, jego proces macierzysty musi zakonczyc swoje dzialanie przed procesem potomnym */
			break;

		default:	/* proces macierzysty */
			break;
	}

	return 0;
}