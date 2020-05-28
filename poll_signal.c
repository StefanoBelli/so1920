#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main() {
	sigset_t everything_blocked;
	sigfillset(&everything_blocked);

	if(sigprocmask(SIG_BLOCK, &everything_blocked, NULL) < 0) {
		perror("sigprocmask");
		return EXIT_FAILURE;
	}

	puts("polling di segnali...");
	while(1) {
		sigset_t pending;
		sigemptyset(&pending); //giusto per essere sicuri

		if(sigpending(&pending) < 0) {
			perror("sigpending");
			return EXIT_FAILURE;
		}

		if(sigprocmask(SIG_UNBLOCK, &pending, NULL) < 0) {
			perror("sigprocmask (unblock)");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
