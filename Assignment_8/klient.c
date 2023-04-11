#define _DEFAULT_SOURCE

#include "msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>

char name[7];	/* nazwa kolejki komunikatow klienta */

mqd_t msgid_c;	/* deskryptor obiektu kolejki komunikatow klienta */
mqd_t msgid_s;	/* deskryptor obiektu kolejki komunikatow serwera */

void my_sighandler(int numer_sygnalu) {	/* obsluga sygnalu SIGINT, ktora bedzie zamykac kolejke serwera i wlasna kolejke komunikator oraz usuwac wlasna kolejke komunikatow */
	exit(EXIT_SUCCESS);
}

void koniec(void) {		/* funkcja wywolana przy zakonczeniu procesu */
	msgClose(msgid_s);
	msgClose(msgid_c);
	msgRm(name);
}

int main() {
	name[0] = '/';			/* poczatkowy ukosnik w nazwie kolejki komunikatow */
	pid_t PID = getpid();
	sprintf(name, "/%d", PID);
	msgid_c = msgCreate(name);

	msgInfo(msgid_c);

	msgid_s = msgOpen(SERWER);

	enum {
		atexit_error = 0
	};
	if(atexit(koniec) != atexit_error) {		/* zmodyfikowanie zakonczenia procesu, aby zamknieto wtedy kolejke serwera i wlasna kolejke komunikatow oraz aby usunieto wlasna kolejke komunikatow */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow atexit() */
		perror("atexit error");
		exit(EXIT_FAILURE);
	}

	if(signal(2, my_sighandler) == SIG_ERR) {	/* ustawienie obslugi sygnalu SIGINT */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow signal() */
		perror("signal error");
		exit(EXIT_FAILURE);
	}

	printf("Prosze wpisac dzialanie oddzielone spacjami: ");

	enum {
		line_max = 22
	};
	char line[line_max];
	srand(time(NULL));							/* seed z czasu biezacego, aby przy kazdym uruchomeiniu programu byly rozne wartosci rand() */
	int min = 1;								/* przedzial generowanych wartosci przez rand() */
	int max = 1000000;						/* 1 zeby sleep trwal minimum jedna mikrosekunde, a 1000000 poniewaz powyzej tej liczby w niektorych systemach jest to blad w usleep() */
	while(fgets(line, sizeof(line) - 1, stdin) != NULL) {			/* czytanie zadanego dzialania */
		komunikat msg;
		msg.PID = PID;
		sscanf(line, "%d %c %d", &msg.liczba_1, &msg.o, &msg.liczba_2);
		msgSend(msgid_s, &msg);

		usleep(rand() % (max + 1 - min) + min);	/* symulowanie roznych szybkosci dzialania programu */

		msgRecv(msgid_c, &msg);

		if(strcmp(msg.blad, "Blad! Dzielenie przez 0!") == 0) {
			printf("Blad! Dzielenie przez 0!\n");
		}
		else if(strcmp(msg.blad, "Blad! Nieznany operator!") == 0) {
			printf("Blad! Nieznany operator!\n");
		}
		else {
			printf("Wynik wykonanego dzialania: %d\n", msg.wynik);
		}

		memset(line, 0, sizeof(line));		/* czyszczenie bufora */
		memset(msg.blad, 0, sizeof(msg.blad));	/* czyszczenie bufora */

		printf("Uwaga! Nie wpisujac nowego dzialania mozna powtorzyc ostatnie dzialanie klikajac enter!\n");
		printf("Prosze wpisac dzialanie oddzielone spacjami: ");
	}

	return EXIT_SUCCESS;
}