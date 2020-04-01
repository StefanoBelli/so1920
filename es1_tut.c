#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define INPUT_MAX 256

int main(int argc, char** argv) {
	char buf[INPUT_MAX];
	fgets(buf, INPUT_MAX, stdin);

	pid_t p = fork();
	if(p == 0)
		printf("(%d) %s\n", getpid(), buf);
	else if(p < 0)
		printf("fork fallita\n");
	else {
		pid_t mepid = getpid();
		printf("(%d) attesa del child: %d...\n", mepid, p);
		int status;
		wait(&status);
		printf("(%d) terminato con codice %d\n", mepid, WEXITSTATUS(status));
	}

	return 0;
}
