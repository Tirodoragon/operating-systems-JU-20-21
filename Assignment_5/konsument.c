#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	if(argc != 3) {													/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu: %s potok_nazwany schowek.txt\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int min = 1;		/* przedzial generowanych wartosci przez rand() */
	int max = 1000000;	/* 1 zeby sleep trwal minimum jedna mikrosekunde, a 1000000 poniewaz powyzej tej liczby w niektorych systemach jest to blad w usleep() */

	int fd[2];	/* tablica deskryptorow plikow */

	char* buffer;					/* utworzenie bufora o zadanym rozmiarze 7 bajtow */
	int bytes = 7;
	buffer = (char *) malloc(bytes);
	if(buffer == NULL) {				/* obsluga bledow malloc() */
		printf("Numer bledu: %d\n", errno);
		perror("malloc error");
		exit(EXIT_FAILURE);
	}

	srand(time(NULL) + getpid());	/* seed z czasu biezacego, aby przy kazdym uruchomieniu programu byly rozne wartosci rand(), + getpid(), aby wartosci byly wieksze niz w producencie */

	if((fd[0] = open(argv[1], O_RDONLY)) == -1) {	/* otwarcie potoku nazwanego */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow open() */
		perror("open error");
		exit(EXIT_FAILURE);
	}

	if((fd[1] = open(argv[2], O_WRONLY | O_CREAT, 0644)) == -1) {	/* otwarcie pliku, ktorego nazwa znajduje sie w argv[2], w ktorym bedziemy umieszczac towar */
		printf("Numer bledu: %d\n", errno);			/* obsluga bledow open() */
		perror("open error");
		exit(EXIT_FAILURE);
	}

	int read_return_of_pipe_in_consumer = -1;									/* wartosc zwracana przez read() */
	int write_return_of_pipe_in_consumer = -1, write_return_of_stdout_in_consumer = -1;				/* wartosc zwracana przez write() */
	while((read_return_of_pipe_in_consumer = read(fd[0], buffer, bytes)) > 0) {					/* pobieranie towaru z potoku */
		write_return_of_pipe_in_consumer = write(fd[1], buffer, read_return_of_pipe_in_consumer);		/* umieszczanie towaru w pliku */
		printf("\nKonsument pobral towar z potoku i umiescil go w schowku Pobrany towar: ");			/* wypisanie na ekran danych przesylanych przez konsumenta */
		fflush(stdout);												/* aby powyzszy printf wypisal sie od razu */
		write_return_of_stdout_in_consumer = write(STDOUT_FILENO, buffer, read_return_of_pipe_in_consumer);
		printf("\n");
		usleep(rand() % (max + 1 - min) + min);									/* symulowanie roznych szybkosci dzialania programu, czekanie na producenta */
	}
	if(read_return_of_pipe_in_consumer == -1) {									/* obsluga bledow read() */
		printf("Numer bledu: %d\n", errno);
		perror("read error");
		exit(EXIT_FAILURE);
	}
	if(write_return_of_pipe_in_consumer == -1) {									/* obsluga bledow write() */
		printf("Numer bledu: %d\n", errno);
		perror("write error");
		exit(EXIT_FAILURE);
	}
	if(write_return_of_stdout_in_consumer == -1) {									/* obsluga bledow write() */
		printf("Numer bledu: %d\n", errno);
		perror("write error");
		exit(EXIT_FAILURE);
	}

	if(close(fd[0]) == -1) {			/* zamkniecie potoku nazwanego */
		printf("Numer bledu: %d\n", errno);
		perror("close error");
		exit(EXIT_FAILURE);
	}

	if(close(fd[1]) == -1) {
		printf("Numer bledu: %d\n", errno);	/* zamkniecie pliku, w ktorym umieszczono towar */
		perror("close error");
		exit(EXIT_FAILURE);
	}

	free(buffer);	/* zwolnienie bufora */

	return 0;
}