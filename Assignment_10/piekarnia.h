/* algorytm piekarni zawiesza sie dla wiekszej ilosci watkow niz ilosc watkow procesora (mozna uzyskac ja z poziomu powloki poleceniem nproc), poniewaz kazdy watek zuzywa 100% procesora, na ktorym jest
   uruchomiony, a przy wiekszej ilosci watkow niz ilosc watkow procesora 2 watki beda dzialaly na jednym watku procesora i zaistnieje walka o czas procesora, a program zakonczy sie poprawnie po bardzo dlugim
   czasie */

#ifndef PIEKARNIA_H
#define PIEKARNIA_H

/* dla zadania 2. liczba watkow musi byc mniejsza lub rowna nproc, a dla zadania 1. moze byc wieksza */
#define LICZBA_WATKOW 8

/* volatile aby zapobiec optymalizacji przez kompilator */
extern volatile int numer[LICZBA_WATKOW];
extern volatile int wybieranie[LICZBA_WATKOW];

void lock(int id);	/* wejscie do sekcji krytycznej wedlug algorytmu piekarni */
void unlock(int id);	/* wyjscie z sekcji krytycznej wedlug algorytmu piekarni */

#endif /* PIEKARNIA_H */