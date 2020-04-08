#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* ask_thread(void* a) {
	char* input;
	scanf("%ms", &input);
	return input;
}

void* print_thread(void* str) {
	puts((char*) str);
}

int main() {
	char* res;

	pthread_t t1;
	pthread_t t2;

	pthread_create(&t1, NULL, ask_thread, NULL);
	pthread_join(t1, (void**)&res);

	pthread_create(&t2, NULL, print_thread, (void*)res);
	pthread_join(t2, NULL);

	free(res);

	return 0;
}
