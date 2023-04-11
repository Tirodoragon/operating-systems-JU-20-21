#include "shmem.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	enum {
		liczba_argumentow = 2
	};
	if(argc != liczba_argumentow && argc != liczba_argumentow + 1) {						/* obsluga blednej liczby argumentow */
		printf("Zla liczba argumentow. Prawidlowe uzycie programu:\n");
		printf("%s c size - tworzy segment pamieci dzielonej o rozmiarze size bajtow\n", argv[0]);
		printf("%s d - usuwa segment pamieci dzielonej\n", argv[0]);
		printf("%s r - wypisuje zawartosc pamieci dzielonej na standardowe wyjscie\n", argv[0]);
		printf("%s w < input.txt - kopiuje zawartosc standardowego wejscia do pamieci dzielonej\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int operacja = *argv[1];
	if(operacja != 'c' && operacja != 'd' && operacja != 'r' && operacja != 'w' && operacja != 'i') {	/* obsluga blednego pierwszego argumentu */
		printf("Zly pierwszy argument. Prawidlowa wartosc: c, d, r lub w\n");
		exit(EXIT_FAILURE);
	}
	int size;
	if(operacja == 'c') {
		size = atoi(argv[2]);
	}
	if(operacja == 'c' && size < 0) {									/* obsluga blednego drugiego argumentu */
		printf("Zly drugi argument. Prawidlowa wartosc: size >= 0\n");
		exit(EXIT_FAILURE);
	}

	enum {
		name_max = 254													/* maksymalna ilosc znakow w nazwie obiektu pamieci dzielonej bez ukosnika */
	};
	enum {
		slash_max = 255													/* maksymalna ilosc znakow w nazwie obiektu pamieci dzielonej z ukosnikiem */
	};
	char shm_name[name_max];																/* nazwa obiektu pamieci dzielonej */
	char slash_name[slash_max];								/* napis, ktory rozpoczyna sie ukosnikiem i do ktorego zostanie dolaczona nazwa obiektu pamieci dzielonej */
	slash_name[0] = '/';
	int fd;																			/* deskryptor obiektu pamieci dzielonej */
	void *ptr;																/* adres poczatku odwzorowanego obszaru pamieci dzielonej */
	char* buffer;																	/* utworzenie bufora o zadanym rozmiarze 1024 bajtow */
	int bytes = 1024;
	buffer = (char *) malloc(bytes);
	if(buffer == NULL) {																	/* obsluga bledow malloc() */
		printf("Numer bledu: %d\n", errno);
		printf("malloc error");
		exit(EXIT_FAILURE);
	}
	int read_return;																	/* wartosc zwracana przez read() */
	int write_return = -1;																	/* wartosc zwracana przez write() */
	enum {
		error = -1
	};
	switch(operacja) {
		case 'c':
			printf("Prosze podac nazwe tworzonej pamieci dzielonej (maksymalnie 253 znakow): ");
			scanf("%253s", shm_name);

			strcat(slash_name, shm_name);	/* dolaczenie do nazwy pamieci dzielonej ukosnika na poczatku */

			fd = shmCreate(slash_name, size);

			ptr = shmMap(fd, size);

			shmClose(ptr, fd, size);

			break;

		case 'd':
			printf("Prosze podac nazwe pamieci dzielonej do usuniecia (maksymalnie 253 znakow): ");
			scanf("%253s", shm_name);

			strcat(slash_name, shm_name);	/* dolaczanie do nazwy pamieci dzielonej ukosnika na poczatku */

			shmRm(slash_name);

			break;

		case 'r':
			printf("Prosze podac nazwe pamieci dzielonej, ktorej zawartosc ma zostac wypisana na standardowe wyjscie (maksymalnie 253 znakow): ");
			scanf("%253s", shm_name);

			strcat(slash_name, shm_name);	/* dolaczenie do nazwy pamieci dzielonej ukosnika na poczatku */

			fd = shmOpen(slash_name);

			size = shmSize(fd);

			ptr = shmMap(fd, size);

			printf("Zawartosc pamieci dzielonej: ");
			fflush(stdout);								/* aby powyzszy printf wypisal sie od razu */
			while((read_return = read(fd, buffer, bytes)) > 0) {			/* wypisanie zawartosci pamieci dzielonej na standardowe wyjscie */
				write_return = write(STDOUT_FILENO, buffer, read_return);
			}
			printf("\n");
			if(read_return == error) {						/* obsluga bledow read() */
				printf("Numer bledu: %d\n", errno);
				perror("read error");
				exit(EXIT_FAILURE);
			}
			if(write_return == error) {						/* obsluga bledow write() */
				printf("Numer bledu: %d\n", errno);
				perror("write error");
				exit(EXIT_FAILURE);
			}

			shmClose(ptr, fd, size);

			break;

		case 'w':
			scanf("%253s", shm_name);

			strcat(slash_name, shm_name);	/* dolaczenie do nazwy pamieci dzielonej ukosnika na poczatku */

			fd = shmOpen(slash_name);

			size = shmSize(fd);	/* sprawdzenie rozmiaru segmentu */

			ptr = shmMap(fd, size);

			printf("Rozmiar segmentu: %d\n", size);
			strcpy(ptr, "");	/* czyszczenie pamieci dzielonej */
			int cur_size = 0;	/* aktualny rozmiar bufora */
			int fgets_count = 0;	/* ilosc wywolan fgets */
			while((fgets(buffer, size, stdin)) != NULL) {									/* kopiowanie zawartosci standardowego wejscia do pamieci dzielonej */
				fgets_count += 1;

				cur_size += strlen(buffer) + 1;

				if(fgets_count != 1) {
					cur_size -= fgets_count - 1;
				}

				if(cur_size > size) {											/* obsluga zbyt dlugiej kopiowanej zawartosci standardowego wejscia */
					printf("Blad! Zbyt dluga kopiowana zawartosc standardowego wejscia do pamieci dzielonej! Maksymalna dlugosc: %d\n", size);
					exit(EXIT_FAILURE);
				}

				strcat(ptr, buffer);	/* kopiowanie zawartosci bufora do pamieci dzielonej */

				strcpy(buffer, "");	/* czyszczenie bufora */
			}

			printf("Pomyslnie skopiowano zawartosc standardowego wejscia do pamieci dzielonej.\n");

			shmClose(ptr, fd, size);

			break;

		case 'i':
			printf("Prosze podac nazwe pamieci dzielonej, o ktorej maja zostac wypisane informacje (maksymalnie 253 znakow): ");
			scanf("%253s", shm_name);

			strcat(slash_name, shm_name);	/* dolaczenie do nazwy pamieci dzielonej ukosnika na poczatku */

			fd = shmOpen(slash_name);

			size = shmSize(fd);

			ptr = shmMap(fd, size);

			shmInfo(fd);

			shmClose(ptr, fd, size);

			break;

		default:
			break;
	}

	free(buffer);	/* zwolnienie bufora */

	return EXIT_SUCCESS;
}