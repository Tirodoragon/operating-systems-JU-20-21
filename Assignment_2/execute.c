#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	printf("%s\n", "\"pierwszy komunikat\"");	/* wypisanie pierwszego komunikatu, pomijajac znak konca linii nie zostanie wyswietlony, poniewaz bufor wyjscia standardowego z poprzedniego programu
							 jest tracony, gdy proces zaczyna wykonywac nowy program, a nie pomijajac znaku konca linii dziala to poprawnie, gdyz powoduje on wyczyszczenie bufora
							 i wypisanie jego zawartosci */

	if(execl("./potomny.x", "./potomny.x", (char *) NULL) == -1) {	/* wykonanie programu potomny.x */
		printf("Numer bledu: %d\n", errno);			 /* obsluga bledow execl() */
		perror("execl error:");
		exit(EXIT_FAILURE);
	}

	printf("%s\n", "\"drugi komunikat\"");	/* wypisanie drugiego komunikatu, nie zostanie wyswietlony, poniewaz funkcja exec bez fork zastepuje caly proces i to co jest po niej juz sie nie wykona */

	return 0;
}