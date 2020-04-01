#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("%s <dir1> ... <dirN>\n", argv[0]);
		return 1;
	}

	for(int i = 1; i < argc; ++i) {
		pid_t chld = fork();
		if(chld == 0) {
			printf("%d:\n", getpid());
			execlp("ls", "ls", argv[i], NULL);
		} else if (chld == -1)
			printf("fork fallita\n");
		
		int status;
		wait(&status);
		printf("\n");
	}

	return 0;
}
