#define _POSIX_C_SOURCE 200809L

#include "procinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	pid_t sid;

	procinfo(argv[0]);

	sid = getsid(0);
	if(sid == -1) {					/* obsluga bledow getsid() */
		printf("Numer bledu: %d\n", errno);
		perror("getsid error");
		exit(EXIT_FAILURE);
	}
	else {
		printf("SID: %d\n", sid);
	}

	sleep(20);

	procinfo(argv[0]);

	return 0;
}