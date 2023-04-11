#include <stdio.h>
#include <string.h>

int strlen2(const char* str) {
	const char* start = str;
	while(*str++);
	return str - start - 1;
}

int main() {
	char* str = "jakis napis";
	printf("%d %d\n", strlen(str), strlen2(str));
}