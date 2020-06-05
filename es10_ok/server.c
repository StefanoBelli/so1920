#ifndef SERVER_FIFO
#define SERVER_FIFO "server_fifo"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <pthread.h>

#define PTHREAD_ROUTINE(rt) ((void* (*)(void*))rt)
#define PTHREAD_ARGUMENT(arg) ((void*)arg)

#define generic_error(x, msg) \
	if(x < 0) { \
		perror(msg); \
		return EXIT_FAILURE; \
	}

#define open_soft_error(fd, what, ...) \
	if(refd < 0) { \
		printf("main thread: unable to open %s\n",what); \
		__VA_ARGS__ \
		continue; }

const int DATA_SIZE = NAME_MAX + NAME_MAX + 2;

int __unbuffered_readline_chomp(int fd, char *buf, int bufsize) {
	int res = -1;
	int i;
	for(
			i = 0; 
			(res = read(fd, buf + i, 1)) == 1 && i < bufsize && buf[i] != '\n'; 
			++i
	) {}

	buf[i] = 0;

	return res; //make it loop forever
}

void ansthr(int* fds) {
	int rfd = fds[0];
	int wfd = fds[1];
	free(fds);
	fds = NULL;

	char buf[7] = { 0 };
	read(rfd, buf, 6);
	
	if(strcmp(buf, "svcreq") == 0)
		write(wfd, "svcok", 5);

	close(rfd);
	close(wfd);
}

int main() {
	generic_error(mkfifo(SERVER_FIFO, O_CREAT | O_EXCL | S_IRUSR | S_IWUSR), "mkfifo");

	int fifofd = open(SERVER_FIFO, O_RDWR);
	generic_error(fifofd, "open");

	puts("reading on \"" SERVER_FIFO "\"");
	
	char buf[NAME_MAX + NAME_MAX + 2] = { 0 };
	int rdres;
	while((rdres = __unbuffered_readline_chomp(fifofd, buf, DATA_SIZE))) {
		if(strcmp(buf, "close") == 0) break;
		else {
			char* read_end_filename = strtok(buf, "\t");
			char* write_end_filename = strtok(NULL, "\t");
			
			int refd = open(read_end_filename, O_RDONLY);
			open_soft_error(refd, read_end_filename);

			int wefd = open(write_end_filename, O_WRONLY);
			open_soft_error(wefd, write_end_filename, close(refd););

			int* fdpair = (int*) malloc(sizeof(int) * 2);
			if(fdpair == NULL) {
				puts("memory exhausted");
				return EXIT_FAILURE;
			}

			fdpair[0] = refd;
			fdpair[1] = wefd;

			pthread_t thr;
			if(pthread_create(&thr, NULL, PTHREAD_ROUTINE(ansthr), PTHREAD_ARGUMENT(fdpair)) < 0) {
				printf("main thread: unable to launch thread (pthread_create: %s)\n", strerror(errno));
				close(refd);
				close(wefd);
			} else
				pthread_detach(thr);
		}
	}

	close(fifofd);
	unlink(SERVER_FIFO);
	generic_error(rdres, "read");
	return EXIT_SUCCESS;
}
