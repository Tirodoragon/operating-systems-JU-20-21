/* algorytm piekarni zawiesza sie dla wiekszej ilosci watkow niz ilosc watkow procesora (mozna uzyskac ja z poziomu powloki poleceniem nproc), poniewaz kazdy watek zuzywa 100% procesora, na ktorym jest
   uruchomiony, a przy wiekszej ilosci watkow niz ilosc watkow procesora 2 watki beda dzialaly na jednym watku procesora i zaistnieje walka o czas procesora, a program zakonczy sie poprawnie po bardzo dlugim
   czasie */

#include "piekarnia.h"

/* instrukcja bariery pamieci, powoduje, ze kompilator wymusza ograniczenie porzadkowania na operacjach pamieci, tzn. ze operacje, ktore zostaly zlecone przed bariera zostana wykonane przed operacjami,
   ktore zostaly zlecone za bariera */
#define BAR_PAM __sync_synchronize()

/* volatile aby zapobiec optymalizacji przez kompilator */
volatile int numer[LICZBA_WATKOW];
volatile int wybieranie[LICZBA_WATKOW];

void lock(int id) {											/* wejscie do sekcji krytycznej wedlug algorytmu piekarni */
	wybieranie[id] = 1;	/* przed dostaniem numeru zmienna "wybieranie" jest ustawiana na true */

	BAR_PAM;	/* bariera pamieci */

	int num_max = 0;

	for(int i = 0; i < LICZBA_WATKOW; i++) {		/* znajdywanie najwiekszego numeru sposrod obecnych watkow */
		int num = numer[i];
		num_max = num > num_max ? num : num_max;
	}

	numer[id] = num_max + 1;	/* przydzielenie nowemu watkowi wartosci najwiekszego numeru + 1 */

	BAR_PAM;	/* bariera pamieci */

	wybieranie[id] = 0;	/* po dostaniu numeru zmienna "wybieranie" jest ustawiana na false */

	for(int j = 0; j < LICZBA_WATKOW; j++) {							/* wejscie do sekcji krytycznej */
		/* warunki algorytmu piekarni */
		while(wybieranie[j]) {
		}

		while(numer[j] != 0 && (numer[j] < numer[id] || (numer[j] == numer[id] && j < id))) {
		}
	}
}

void unlock(int id) {	/* wyjscie z sekcji krytycznej wedlug algorytmu piekarni */
	BAR_PAM;	/* bariera pamieci */

	numer[id] = 0;	/* wyzerowanie odpowiedniej wartosci */
}