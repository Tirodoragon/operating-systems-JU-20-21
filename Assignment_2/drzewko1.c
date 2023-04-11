#include "procinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	int i, count = 0;	/* i - iterator for, count - licznik procesow potomnych */

	strncpy(argv[0], "parent", strlen(argv[0]));	/* zmiana nazwy programu argv[0] na "parent" dla procesu macierzystego */

	procinfo(argv[0]);

	printf("Procesy potomne:\n");

	pid_t pid;	/* wartosc zwracana przez fork() */

	for(i = 0; i < 3; i++) {
		pid = fork();	/* tworzenie procesu potomnego, bedacego kopia procesu macierzystego */

		switch(pid) {
			case -1:					/* obsluga bledow fork() */
				printf("Numer bledu: %d\n", errno);
				perror("fork error");
				exit(EXIT_FAILURE);
				break;

			case 0:							/* proces potomny */
				count = 0;					/* nowy proces potomny nie ma procesow potomnych */
				sleep(1);					/* aby procesy pojawialy sie na ekranie grupowane pokoleniami od najstarszego do najmlodszego */
				strncpy(argv[0], "child", strlen(argv[0]));	 /* zmiana nazwy programu argv[0] na "child" dla procesu potomnego */
				procinfo(argv[0]);				/* wypisanie identyfikatorow procesu potomnego */
				i = 2;						/* aby spelniac zalozenie, ze proces macierzysty 1 posiada trzy procesy potomne 2, 3 i 4 */
				break;

			default:					/* proces macierzysty */
				count++;				/* zwiekszenie liczby procesow potomnych o 1 */
				break;
		}
	}

	while (count > 0) {					/* czekanie na wszystkie procesy potomne, jezeli jakies sa */
		if((wait(NULL)) == -1) {			/* obsluga bledow wait() */
			printf("Numer bledu: %d\n", errno);
			perror("wait error");
			exit(EXIT_FAILURE);
		}
		else {			/* zmniejszenie liczby procesow potomnych o 1 */
			count -= 1;
		}
	}

	return 0;
}