#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	if(argc != 3) {													/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu: %s plik_zrodlowy plik_docelowy\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd[2];	/* tablica deskryptorow plikow */

	fd[0] = open(argv[1], O_RDONLY); 		/* otwarcie pliku, ktorego nazwa znajduje sie w argv[1], z ktorego bedziemy kopiowac zawartosc */
	if(fd[0] == -1) {				/* obsluga bledow open() */
		printf("Numer bledu: %d\n", errno);
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}
	fd[1] = open(argv[2], O_WRONLY | O_CREAT, 0644);	/* otwarcie pliku, ktorego nazwa znajduje sie w argv[2], do ktorego bedziemy kopiowac zawartosc */
	if(fd[1] == -1) {									/* obsluga bledow open() */
		printf("Numer bledu: %d\n", errno);
		perror(argv[2]);
		exit(EXIT_FAILURE);
	}
	
	char* buffer;					/* utworzenie bufora o zadanym rozmiarze 1024 bajtow */
	int bytes = 1024;
	buffer = (char *) malloc(bytes);
	if(buffer == NULL) {				/* obsluga bledow malloc() */
		printf("Numer bledu: %d\n", errno);
		perror("malloc error");
		exit(EXIT_FAILURE);
	}

	int read_return = -1;						/* czytanie i pisanie */
	int write_return = -1;
	while((read_return = read(fd[0], buffer, bytes)) > 0) {
		write_return = write(fd[1], buffer, read_return);
	}
	if(read_return == -1) {				/* obsluga bledow read() */
		printf("Numer bledu: %d\n", errno);
		perror("read error");
		exit(EXIT_FAILURE);
	}
	if(write_return == -1) {			/* obsluga bledow write() */
		printf("Numer bledu: %d\n", errno);
		perror("write error");
		exit(EXIT_FAILURE);
	}

	if(close(fd[0]) == -1) {			/* zamkniecie deskryptora */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close fd[0] error");
		exit(EXIT_FAILURE);
	}
	if(close(fd[1]) == -1) {			/* zamkniecie deskryptora */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close fd[1] error");
		exit(EXIT_FAILURE);
	}

	free(buffer);	/* zwolnienie bufora */

	return 0;
}