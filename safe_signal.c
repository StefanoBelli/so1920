#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void handler(int sig) {
	printf("received signal %d, doing some work...\n", sig);
	sleep(3);
}

int main() {
	struct sigaction act;
	act.sa_handler = handler;
	sigfillset(&act.sa_mask);
	if(sigaction(SIGINT, &act, NULL) < 0) {
		perror("sigaction");
		return EXIT_FAILURE;
	}

	char buf;
	while(1) {
		if(read(STDIN_FILENO, &buf, 1) < 0 && errno != EINTR)
			perror("read");
	}

	return EXIT_SUCCESS;
}
