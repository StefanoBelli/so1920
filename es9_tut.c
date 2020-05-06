#define F "file.txt"
#define N 5
#define W 6

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define errdie(res) if(res == -1) { \
	printf("[%d] " #res ": %s\n", getpid(), strerror(errno)); \
	return EXIT_FAILURE; \
}

int do_work(int fd, int procidx) {
	struct flock lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_type = F_WRLCK;

	errdie(fcntl(fd, F_SETLKW, &lock)); //inizio lock
	char buf[20] = { 0 };
	snprintf(buf, 19, "%d=procidx\n", procidx);

	size_t real_bufsize = strlen(buf);

	for(int i = 0; i < W; ++i)
		errdie(write(fd, buf, real_bufsize));
	
	lock.l_type = F_UNLCK;
	errdie(fcntl(fd, F_SETLK, &lock)); //fine lock

	errdie(close(fd));
	return EXIT_SUCCESS;
}

int main() {
	int fd = open(F, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	errdie(fd);

	pid_t pids[N];
	for(int i = 0; i < N; ++i) {
		pids[i] = fork();
		if(pids[i] == -1)
			perror("fork()");
		else if(pids[i] == 0)
			exit(do_work(fd, i));
	}

	for(int i = 0; i < N; ++i) {
		if(pids[i] == -1)
			printf("fork %d fallita\n", i + 1);
		else {
			int status;
			errdie(wait(&status));
			printf("%d terminato con codice %d\n", i, WEXITSTATUS(status));
		}
	}

	errdie(close(fd));
	return EXIT_SUCCESS;
}
