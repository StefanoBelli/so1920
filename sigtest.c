#include <signal.h>
#include <stdio.h>
#include <pthread.h>
#include <syscall.h>
#include <unistd.h>

void handler(int s) {
	printf("sig %d handled by: %ld\n", s, syscall(SYS_gettid));
}

void* thr(void* p) {
//	sigset_t set;
//	sigfillset(&set);
//	sigprocmask(SIG_BLOCK, &set, NULL);
	while(1){}
}

int main() {
	signal(SIGINT, handler);

	pthread_t t;
	pthread_create(&t, NULL, thr, NULL);

	while(1) {}

	return 0;
}
