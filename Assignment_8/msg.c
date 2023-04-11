#include "msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>

int msgCreate(const char *name) {
	printf("Uruchomiono funkcje tworzaca kolejke komunikatow o nazwie %s.\n", name);

	enum {
		mq_access_permissions = 0644
	};
	struct mq_attr attr;																	/* struktura atrybutow */
	enum {
		zero
	};
	enum {
		max = 10
	};
	attr.mq_flags = zero;
	attr.mq_maxmsg = max;
	attr.mq_msgsize = sizeof(komunikat);
	attr.mq_curmsgs = zero;
	enum {
		error = -1
	};
	mqd_t msgid;																		/* deskryptor obiektu kolejki komunikatow */
	if((msgid = mq_open(name, O_RDWR | O_CREAT | O_EXCL, mq_access_permissions, &attr)) == (mqd_t) error) {							/* obsluga bledow mq_open() */
		printf("Blad w funkcji tworzacej kolejke komunikatow o nazwie %s. Kolejka komunikatow o nazwie %s nie zostala utworzona.\n", name, name);
		printf("Numer bledu: %d\n", errno);
		perror("mq_open error");
		exit(EXIT_FAILURE);
	}

	printf("Kolejka komunikatow o nazwie %s zostala utworzona. Jej deskryptor to: %d\n", name, msgid);

	return msgid;
}

int msgOpen(const char *name) {
	printf("Uruchomiono funkcje otwierajaca kolejke komunikatow o nazwie %s.\n", name);

	enum {
		error = -1
	};
	mqd_t msgid;																		/* deskryptor obiektu kolejki komunikatow */
	if((msgid = mq_open(name, O_RDWR)) == (mqd_t) error) {				/* O_RDWR zamiast oflag do wyboru trybu do odbierania lub nadawania, bo w prototypie funkcji w zestawie tego nie bylo */
		printf("Blad w funkcji otwierajacej kolejke komunikatow o nazwie %s. Kolejka komunikatow o nazwie %s nie zostala otwarta.\n", name, name);	/* obsluga bledow mq_open() */
		printf("Numer bledu: %d\n", errno);
		perror("mq_open error");
		exit(EXIT_FAILURE);
	}

	printf("Kolejka komunikatow o nazwie %s zostala otwarta. Jej deskryptor to: %d\n", name, msgid);

	return msgid;
}

void msgClose(int msgid) {
	printf("Uruchomiono funkcje zamykajaca dostep do kolejki o deskryptorze %d.\n", msgid);

	enum {
		error = -1
	};
	if(mq_close(msgid) == error) {																	/* obsluga bledow mq_close() */
		printf("Blad w funkcji zamykajacej dostep do kolejki o deskryptorze %d. Dostep do kolejki o deskryptorze %d nie zostal zamkniety.\n", msgid, msgid);
		printf("Numer bledu: %d\n", errno);
		perror("mq_close error");
		exit(EXIT_FAILURE);
	}

	printf("Dostep do kolejki o deskryptorze %d zostal zamkniety.\n", msgid);
}

void msgRm(const char *name) {
	printf("Uruchomiono funkcje usuwajaca kolejke komunikatow o nazwie %s.\n", name);

	enum {
		error = -1
	};
	if(mq_unlink(name) == error) {																/* obsluga bledow mq_unlink() */
		printf("Blad w funkcji usuwajacej kolejke komunikatow o nazwie %s. Kolejka komunikatow o nazwie %s nie zostala usunieta.\n", name, name);
		printf("Numer bledu: %d\n", errno);
		perror("mq_unlink error");
		exit(EXIT_FAILURE);
	}

	printf("Kolejka komunikatow o nazwie %s zostala usunieta.\n", name);
}

void msgSend(int msgid, komunikat *msg) {
	printf("Uruchomiono funkcje wysylajaca komunikat %d %c %d do kolejki o identyfikatorze %d.\n", msg->liczba_1, msg->o, msg->liczba_2, msgid);

	enum {
		error = -1
	};
	if(mq_send(msgid, (const char *) msg, sizeof(komunikat), 0) == error) {										/* obsluga bledow mq_send() */
		printf("Blad w funkcji wysylajacej komunikat %d %c %d do kolejki o identyfikatorze %d. ", msg->liczba_1, msg->o, msg->liczba_2, msgid);
		printf("Komunikat %d %c %d nie zostal wyslany do kolejki o identyfikatorze %d.\n", msg->liczba_1, msg->o, msg->liczba_2, msgid);
		printf("Numer bledu: %d\n", errno);
		perror("mq_send error");
		exit(EXIT_FAILURE);
	}

	printf("Komunikat %d %c %d zostal wyslany do kolejki o identyfikatorze %d.\n", msg->liczba_1, msg->o, msg->liczba_2, msgid);
}

void msgRecv(int msgid, komunikat *msg) {
	printf("Uruchomiono funkcje odbierajaca z kolejki %d komunikat liczba_1 o liczba_2 i zapisujaca go.\n", msgid);

	unsigned int prio;
	enum {
		error = -1
	};
	if(mq_receive(msgid, (char *) msg, sizeof(komunikat), &prio) == error) {									/* obsluga bledow mq_receive() */
		printf("Blad w funkcji odbierajacej z kolejki %d komunikat liczba_1 o liczba_2 i zapisujacej go. ", msgid);
		printf("Komunikat liczba_1 o liczba_2 nie zostal odebrany z kolejki %d i nie zostal zapisany.\n", msgid);
		printf("Numer bledu: %d\n", errno);
		perror("mq_receive error");
		exit(EXIT_FAILURE);
	}

	printf("Komunikat %d %c %d zostal odebrany z kolejki %d i zostal zapisany.\n", msg->liczba_1, msg->o, msg->liczba_2, msgid);
}

void msgInfo(int msgid) {
	printf("Uruchomiono funkcje wypisujaca wszystkie informacje o kolejce komunikatow %d uzyskane przy pomocy mq_getattr().\n", msgid);

	enum {
		error = -1
	};
	struct mq_attr attr;															/* struktura atrybutow */
	if(mq_getattr(msgid, &attr) == error) {													/* obsluga bledow mq_getattr() */
		printf("Blad w funkcji wypisujacej wszystkie informacje o kolejce komunkatow %d uzyskane przy pomocy mq_getattr(). ", msgid);
		printf("Wszystkie informacje o kolejce komunikatow %d uzyskane przy pomocy mq_getattr() nie zostaly wypisane.\n", msgid);
		printf("Numer bledu: %d\n", errno);
		perror("mq_getattr error");
		exit(EXIT_FAILURE);
	}

	printf("Wszystkie informacje o kolejce komunikatow %d uzyskane przy pomocy mc_getattr():\n", msgid);

	printf("Flaga: %d\n", attr.mq_flags);
	printf("Maksymalna ilosc komunikatow: %d\n", attr.mq_maxmsg);
	printf("Maksymalna wielkosc komunikatu: %d\n", attr.mq_msgsize);
	printf("Obecna ilosc komunikatow: %d\n", attr.mq_curmsgs);
}