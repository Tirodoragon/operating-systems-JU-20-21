#ifndef MSG_H
#define MSG_H

#include <sys/types.h>

#define SERWER "/serwer"
#define MAX 25

typedef struct komunikat {
	pid_t PID;
	int liczba_1;
	char o;		/* operator */
	int liczba_2;
	int wynik;
	char blad[MAX];
} komunikat;

int msgCreate(const char *name);		/* tworzy kolejke komunikatow o nazwie name i zwraca do niej deskryptor */
int msgOpen(const char *name);			/* otwiera kolejke komunikatow o nazwie name i zwraca do niej deskryptor */
void msgClose(int msgid);			/* zamyka dostep do kolejki o deskryptorze msgid */
void msgRm(const char *name);			/* usuwa kolejke komunikatow o nazwie name */
void msgSend(int msgid, komunikat *msg);	/* wysyla komunikat wskazywany przez msg do kolejki o identyfikatorze msgid */
void msgRecv(int msgid, komunikat *msg);	/* odbiera z kolejki msgid komunikat i zapisuje go do msg */
void msgInfo(int msgid);			/* wypisuje wszystkie informacje o kolejce komunikatow uzyskane przy pomocy mq_getattr() */

#endif /* MSG_H */