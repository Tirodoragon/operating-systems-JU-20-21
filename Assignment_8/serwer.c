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

mqd_t msgid_s;	/* deskryptor obiektu kolejki komunikatow serwera */

void my_sighandler(int numer_sygnalu) {	/* obsluga sygnalu SIGINT, ktora bedzie zamykac i usuwac wlasna kolejke komunikatow */
	exit(EXIT_SUCCESS);
}

void koniec(void) {		/* funkcja wywolywana przy zakonczeniu procesu */
	msgClose(msgid_s);
	msgRm(SERWER);
}

int main() {
	msgid_s = msgCreate(SERWER);

	enum {
		atexit_error = 0
	};
	if(atexit(koniec) != atexit_error) {		/* zmodyfikowanie zakonczenia procesu, aby zamknieto i usunieto wtedy wlasna kolejke komunikatow */
		printf("Numer bledu: %d\n", errno);	/* obsluga bledow atexit() */
		perror("atexit error");
		exit(EXIT_FAILURE);
	}

	if(signal(2, my_sighandler) == SIG_ERR) {		/* ustawienie obslugi sygnalu SIGINT */
		printf("Numer bledu: %d\n", errno);		/* obsluga bledow signal() */
		perror("Funkcja signal ma problem z SIGINT");
		exit(EXIT_FAILURE);
	}

	msgInfo(msgid_s);

	pid_t PID;
	char name[7];									/* nazwa kolejki komunikatow klienta */
	name[0] = '/';									/* poczatkowy ukosnik w nazwie kolejki komunikatow */
	mqd_t msgid_c;									/* deskryptor obiektu kolejki komunikatow klienta */
	int liczba_1;
	char o;										/* operator */
	int liczba_2;
	int wynik;
	int zero = 0;									/* zmienna wykorzystywana do sprawdzenia, czy wystapil blad */
	srand(time(NULL));								/* seed z czasu biezacego, aby przy kazdym uruchomieniu programu byly rozne wartosci rand() */
	int min = 1;									/* przedzial generowanych wartosci przez rand() */
	int max = 1000000;						/* 1 zeby sleep trwal minimum jedna mikrosekunde, a 1000000 poniewaz powyzej tej liczby w niektorych systemach jeset to blad w usleep() */
	while(1) {
		komunikat msg;
		msgRecv(msgid_s, &msg);

		PID = msg.PID;
		sprintf(name, "/%d", PID);
		msgid_c = msgOpen(name);

		liczba_1 = msg.liczba_1;
		o = msg.o;
		liczba_2 = msg.liczba_2;
		switch(o) {
			case '+':
				wynik = liczba_1 + liczba_2;

				break;

			case '-':
				wynik = liczba_1 - liczba_2;

				break;

			case '*':
				wynik = liczba_1 * liczba_2;

				break;

			case '/':
				if(liczba_2 == 0) {					/* obsluga bledu dzielenia przez 0 */
					printf("Blad! Dzielenie przez 0!\n");

					sprintf(msg.blad, "Blad! Dzielenie przez 0!");

					zero = 1;	/* wystapienie bledu */
				}
				else {
					wynik = liczba_1 / liczba_2;
				}

				break;

			default:						/* obsluga blednego operatora */
				printf("Blad! Nieznany operator!\n");

				sprintf(msg.blad, "Blad! Nieznany operator!");

				zero = 1;	/* wystapienie bledu */

				break;
		}

		if(!zero) {							/* jezeli nie wystapil blad */
			msg.wynik = wynik;

			printf("Wynik wykonanego dzialania: %d\n", msg.wynik);
		}

		usleep(rand() % (max + 1 - min) + min);	/* symulowanie roznych szybkosci dzialania programu */

		msgSend(msgid_c, &msg);

		zero = 0;
		memset(msg.blad, 0, sizeof(msg.blad));	/* czyszczenie bufora */

		msgClose(msgid_c);
	}

	return EXIT_SUCCESS;
}