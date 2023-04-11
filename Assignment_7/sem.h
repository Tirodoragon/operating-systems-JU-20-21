#ifndef SEM_H
#define SEM_H

#include <semaphore.h>

typedef sem_t *semid;

semid semCreate(const char *name, int val);	/* tworzy semafor z wartoscia poczatkowa val */
void semRemove(const char *name);		/* usuwa semafor */
semid semOpen(const char *name);		/* uzyskuje dostep do istniejacego semafora */
void semClose(semid sem);			/* zamyka semafor w danym procesie */
void semP(semid sem);				/* opuszcza semafor: zmniejsza wartosc semafora o 1 lub czeka */
void semV(semid sem);				/* podnosi semafor: zwieksza wartosc semafora o 1 lub wznawia czekajacy proces */
void semInfo(semid sem);			/* funkcja wypisuje informacje o wartosci semafora */

#endif /* SEM_H */