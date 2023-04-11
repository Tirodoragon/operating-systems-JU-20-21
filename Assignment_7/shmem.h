#ifndef SHMEM_H
#define SHMEM_H

#define NELE 20	/* rozmiar elementu bufora (jedostki towaru) w bajtach */
#define NBUF 5	/* liczba elementow bufora */

typedef char towar;

typedef struct cykliczny {
	int size;		/* calkowity rozmiar bufora */
	int beg;		/* pierwszy element do odczytu */
	int end;		/* element za ostatnim zapisanym elementem */
	int koniec;		/* miejsce w porcji danych, gdzie znajduje sie \0 */
	towar buf[NBUF][NELE];	/* dane, wlasny typ 'towar' rownowazny 'char' */
} cykliczny;

int shmCreate(const char* name, int size);	/* funkcja tworzy pamiec dzielona o nazwie name i rozmiarze size, zwraca deskryptor obiektu pamieci dzielonej */
int shmOpen(const char* name);			/* funkcja otwiera obiekt pamieci dzielonej i zwraca jego deskryptor */
void shmRm(const char* name);			/* usuwa obiekt pamieci dzielonej o nazwie name */
void* shmMap(int fd, int size);			/* odwzorowuje obiekt pamieci dzielonej o deskryptorze fd i rozmiarze size w przestrzen adresowa procesu, zwraca adres poczatku odwzorowanego obszaru pamieci
                                                   dzielonej */
void shmClose(void *ptr, int fd, int size);	/* usuwa odwzorowanie obiektu pamieci dzielonej i zamyka powiazany z nim deskryptor */
int shmSize(int fd);				/* zwraca rozmiar pamieci dzielonej okreslonej deskryptorem fd */
void shmInfo(int fd);				/* wypisuje informacje o pamieci dzielonej: prawa dostepu, rozmiar, wlasciciel, etc. */

#endif /* SHMEM_H */