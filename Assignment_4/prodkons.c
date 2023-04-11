#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

void producent(char* argv[], int min, int max, int* fd, char* buffer, int bytes) {
	srand(time(NULL));	/* seed z czasu biezacego, aby przy kazdym uruchomieniu programu byly rozne wartosci rand() */

	if(close(fd[0]) == -1) { 			/* zamkniecie nieuzywanego przez producenta deskryptora potoku do odczytu */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close error");
		exit(EXIT_FAILURE);
	}

	if((fd[2] = open(argv[1], O_RDONLY)) == -1) {	/* otwarcie pliku, ktorego nazwa znajduje sie w argv[1], z ktorego bedziemy pobierac surowiec */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow open() */
		perror("open error");
		exit(EXIT_FAILURE);
	}

	int read_return_of_pipe_in_producer = -1;									/* wartosc zwracana przez read() */
	int write_return_of_pipe_in_producer = -1, write_return_of_stdout_in_producer = -1;				/* wartosc zwracana przez write() */
	while((read_return_of_pipe_in_producer = read(fd[2], buffer, bytes)) > 0) {					/* pobieranie surowca z pliku */
		write_return_of_pipe_in_producer = write(fd[1], buffer, read_return_of_pipe_in_producer);		/* wstawianie surowca jako towar do potoku */
		printf("\nProducent pobral surowiec z magazynu i wstawil go jako towar do potoku. Pobrany surowiec: ");	/* wypisanie na ekran danych przesylanych przez producenta */
		fflush(stdout);												/* aby powyzszy printf wypisal sie od razu */
		write_return_of_stdout_in_producer = write(STDOUT_FILENO, buffer, read_return_of_pipe_in_producer);
		printf("\n");
		usleep(rand() % (max + 1 - min) + min);									/* symulowanie roznych szybkosci dzialania programu, czekanie na proces potomny */
	}
	if(read_return_of_pipe_in_producer == -1) {									/* obsluga bledow read() */
		printf("Numer bledu: %d\n", errno);
		perror("read error");
		exit(EXIT_FAILURE);
	}
	if(write_return_of_pipe_in_producer == -1) {									/* obsluga bledow write() */
		printf("Numer bledu: %d\n", errno);
		perror("read error");
		exit(EXIT_FAILURE);
	}
	if(write_return_of_stdout_in_producer == -1) {									/* obsluga bledow write() */
		printf("Numer bledu: %d\n", errno);
		perror("write error");
		exit(EXIT_FAILURE);
	}

	if(close(fd[1]) == -1) {			/* zamkniecie deskryptora potoku do zapisu */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close error");
		exit(EXIT_FAILURE);
	}

	if(close(fd[2]) == -1) {			/* zamkniecie pliku, z ktorego pobrano surowiec */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close error");
		exit(EXIT_FAILURE);
	}

	if(wait(NULL) == -1) {				/* czekanie na proces potomny */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow wait() */
		perror("wait error");
		exit(EXIT_FAILURE);
	}

	free(buffer);	/* zwolnienie bufora */

	exit(EXIT_SUCCESS);
}

void konsument(char* argv[], int min, int max, int* fd, char* buffer, int bytes) {
	srand(time(NULL) + getpid());	/* seed z czasu biezacego, aby przy kazdym uruchomieniu programu byly rozne wartosci rand(), + getpid(), aby wartosci byly wieksze niz w producencie */

	if(close(fd[1]) == -1) {			/* zamkniecie nieuzywanego przez producenta deskryptora potoku do zapisu */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close error");
		_exit(EXIT_FAILURE);
	}
	
	if((fd[3] = open(argv[2], O_WRONLY | O_CREAT, 0644)) == -1) {	/* otwarcie pliku, ktorego nazwa znajduje sie w argv[2], w ktorym bedziemy umieszczac towar */
		printf("Numer bledu: %d\n", errno);							/* obsluga bledow open() */
		perror("open error");
		_exit(EXIT_FAILURE);
	}

	int read_return_of_pipe_in_consumer = -1;									/* wartosc zwracana przez read() */
	int write_return_of_pipe_in_consumer = -1, write_return_of_stdout_in_consumer = -1;				/* wartosc zwracana przez write() */
	while((read_return_of_pipe_in_consumer = read(fd[0], buffer, bytes)) > 0) {					/* pobieranie towaru z potoku */
		write_return_of_pipe_in_consumer = write(fd[3], buffer, read_return_of_pipe_in_consumer);		/* umieszczanie towaru w pliku */
		printf("\nKonsument pobral towar z potoku i umiescil go w schowku. Pobrany towar: ");			/* wypisanie na ekran danych przesylanych przez konsumenta */
		fflush(stdout);												/* aby powyzszy printf wypisal sie od razu */
		write_return_of_stdout_in_consumer = write(STDOUT_FILENO, buffer, read_return_of_pipe_in_consumer);
		printf("\n");
		usleep(rand() % (max + 1 - min) + min);									/* symulowanie roznych szybkosci dzialania programu, czekanie na proces macierzysty */
	}
	if(read_return_of_pipe_in_consumer == -1) {									/* obsluga bledow read() */
		printf("Numer bledu: %d\n", errno);
		perror("read error");
		_exit(EXIT_FAILURE);
	}
	if(write_return_of_pipe_in_consumer == -1) {									/* obsluga bledow write() */
		printf("Numer bledu: %d\n", errno);
		perror("write error");
		_exit(EXIT_FAILURE);
	}
	if(write_return_of_stdout_in_consumer == -1) {									/* obsluga bledow write() */
		printf("Numer bledu: %d\n", errno);
		perror("write error");
		_exit(EXIT_FAILURE);
	}

	if(close(fd[0]) == -1) {			/* zamkniecie deskryptora potoku do odczytu */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close error");
		_exit(EXIT_FAILURE);
	}

	if(close(fd[3]) == -1) {			/* zamkniecie pliku, w ktorym umieszczono towar */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow close() */
		perror("close error");
		_exit(EXIT_FAILURE);
	}

	free(buffer);	/* zwolnienie bufor */

	_exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
	if(argc != 3) {													/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu: %s magazyn.txt schowek.txt\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int min = 1;		/* przedzial generowanych wartosci przez rand() */
	int max = 1000000;	/* 1 zeby sleep trwal minimum jedna mikrosekunde, a 1000000 poniewaz powyzej tej liczby w niektorych systemach jest to blad w usleep() */

	int fd[4];	/* tablica deskryptorow plikow */

	char* buffer;					/* utworzenie bufora o zadanym rozmiarze 7 bajtow */
	int bytes = 7;
	buffer = (char *) malloc(bytes);
	if(buffer == NULL) {				/* obsluga bledow malloc() */
		printf("Numer bledu: %d\n", errno);
		perror("malloc error");
		exit(EXIT_FAILURE);
	}

	if(pipe(fd) == -1) {				/* obsluga bledow pipe() */
		printf("Numer bledu: %d\n", errno);
		perror("pipe error");
		exit(EXIT_FAILURE);
	}

	switch(fork()) {						/* utworzenie procesu konsumenta */
		case -1:						/* obsluga bledow fork() */
			printf("Numer bledu: %d\n", errno);
			perror("fork error");
			exit(EXIT_FAILURE);
			break;

		case 0:							/* proces potomny - konsument */
			konsument(argv, min, max, fd, buffer, bytes);
			break;

		default:						/* proces macierzysty - producent */
			producent(argv, min, max, fd, buffer, bytes);
			break;
	}

	return 0;
}