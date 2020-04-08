#include <stdio.h>
#include <pthread.h>

int sync_thread = 0;

void* thread_function(void* arg) {
	puts("secondo thread: creato");
	sync_thread = 1;
}

int main() {
	pthread_t tid;
	pthread_create(&tid, NULL, thread_function, NULL);
	while(!sync_thread){}
	printf("main thread: thread appena creato");
	pthread_join(tid, NULL);

	return 0;
}
