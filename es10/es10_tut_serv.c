#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>

#define error_exit(res, msg) if(res < 0) { \
	perror(msg); exit(EXIT_FAILURE); }

#define log_thread_error(msg) \
	printf("thread %ld - %s: %s\n", syscall(SYS_gettid), msg, strerror(errno))

void resp(int fd) {
	char buf[sizeof("svcreq")] = { 0 };
	
	if(read(fd, buf, sizeof("svcreq")) == -1) {
		log_thread_error("read");
		close(fd);
		return;
	}

	if(write(fd, "svcok", sizeof("svcok")) == -1) {
		log_thread_error("write");
		close(fd);
		return;
	}

	close(fd);
}

int read_filename(int fd, char* buf, int size) {
	int i = 0;
	int res;
	while((res = read(fd, buf + i, 1)) == 1 && buf[i] != '\n' && i < size) { ++i; }
	buf[i] = 0;

	return res > 0;
}

int main() {
	int check = mkfifo("server_fifo", O_CREAT | O_EXCL | S_IRUSR | S_IWUSR);
	error_exit(check, "mkfifo");

	int fd = open("server_fifo",O_RDONLY);
	error_exit(fd, "open");

	char cwd[PATH_MAX];
	getcwd(cwd, PATH_MAX);
	
	printf("reading on FIFO at \"%s/server_fifo\"\n", cwd);

	char buf[NAME_MAX] = { 0 };
	while(read_filename(fd, buf, NAME_MAX) > 0) {
		if(strcmp(buf, "close") == 0) break;
		int clientfd = open(buf, O_RDWR);

		if(clientfd < 0)
			printf("main thread: unable to open(%s): %s\n", buf, strerror(errno));
		else {
			pthread_t thr;
			if(pthread_create(&thr, NULL, (void* (*)(void*))resp, (void*)clientfd) < 0) {
				printf("main thread: unable to create new thread: %s\n", strerror(errno));
			}

			if(pthread_detach(thr)) {
				printf("WARNING: main thread: unable to detach thread - pthread_detach: %s\n", strerror(errno));
			}
		}
		memset(buf, 0, NAME_MAX);
	}

	close(fd);
	unlink("server_fifo");
	return EXIT_SUCCESS;
}
