#include <stdio.h>
#include <pthread.h>

typedef struct {
	char* buf;
	int size;
} ask_params;

void* ask_thread(void* ptr) {
	ask_params* p = (ask_params*) ptr;
	fgets(p->buf, p->size, stdin);
}

void* print_thread(void* ptr) {
	printf("%s",(char*)ptr);
}

int main() {
	char var[256];

	ask_params p;
	p.buf = var;
	p.size = 256;

	pthread_t t1;
	pthread_t t2;

	pthread_create(&t1, NULL, ask_thread, (void*)&p);
	pthread_join(t1, NULL);

	pthread_create(&t2, NULL, print_thread, (void*)p.buf);
	pthread_join(t2, NULL);

	return 0;
}
