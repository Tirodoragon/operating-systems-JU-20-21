#define _POSIX_C_SOURCE 200809L

#include "procinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int procinfo(const char* name) {
	uid_t UID = getuid();
	gid_t GID = getgid();
	pid_t PID = getpid();
	pid_t PPID = getppid();
	pid_t PGID = getpgid(PID);
	if(PGID == -1) {				/* obsluga bledow getpgid() */
		printf("Numer bledu: %d\n", errno);
		perror("getpgid error");
		exit(EXIT_FAILURE);
	}

	printf("Name = %s, UID = %i, GID = %i, PID = %i, PPID = %i, PGID = %i\n", name, UID, GID, PID, PPID, PGID);

	return 0;
}