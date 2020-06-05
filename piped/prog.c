#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int pipefds[2];
	pipe(pipefds);

	//ls .. | grep sort
	
	pid_t p1 = fork();
	if(p1 == 0) {
		close(STDOUT_FILENO);
		close(pipefds[0]);
		dup(pipefds[1]);
		execlp("ls", "ls", "..", NULL);
	} else {
		pid_t p2 = fork();
		if(p2 == 0) {
			close(STDIN_FILENO);
			close(pipefds[1]);
			dup(pipefds[0]);
			execlp("sort", "sort", NULL);
		}
		
		close(pipefds[0]);
		close(pipefds[1]);
	}

	return 0;
}
