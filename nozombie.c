#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#ifdef NO_ZOMBIE
#include <signal.h>
#endif

int main() {

#ifdef NO_ZOMBIE
	signal(SIGCHLD, SIG_IGN);	//le strutture dati del processo child 
								//non verranno mantenute
#endif 

	pid_t p = fork();
	if(p == 0) {
		sleep(2);
		exit(12);
	} else {
		int status;
		waitpid(p, &status, 0);
		printf("exit: %d\n", WEXITSTATUS(status));
	}

	return 0;
}
