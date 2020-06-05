#ifndef DFL_THRS
#define DFL_THRS 2
#endif

#ifndef SERVER_FIFO
#define SERVER_FIFO "server_fifo"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <syscall.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PTHREAD_ROUTINE(rt) ((void* (*)(void*))rt)
#define PTHREAD_ARGUMENT(arg) ((void*)arg)

#define log_thread_error(msg) \
	printf("thread %ld - %s: %s\n", syscall(SYS_gettid), msg, strerror(errno))

const int DATA_SIZE = NAME_MAX + NAME_MAX + 2;

int askthr(int i) {
	char fifo_w[NAME_MAX] = { 0 };
	char fifo_r[NAME_MAX] = { 0 };

	snprintf(fifo_w, NAME_MAX, "client_fifo_%d", i);
	snprintf(fifo_r, NAME_MAX, "client_fifo_read_%d", i);

	if(mkfifo(fifo_w, O_CREAT | O_EXCL | S_IRUSR | S_IWUSR) < 0)
		return 0;

	if(mkfifo(fifo_r, O_CREAT | O_EXCL | S_IRUSR | S_IWUSR) < 0)
		return 0;
	
	int sfd = open(SERVER_FIFO, O_WRONLY);
	if(sfd < 0) {
		printf("openerr\n");
		return 0;
	}
	
	char sendata[NAME_MAX + NAME_MAX + 2] = { 0 };
	snprintf(sendata, DATA_SIZE, "%s\t%s\n", fifo_w, fifo_r);
	write(sfd, sendata, strlen(sendata));
	close(sfd);

	int wfd = open(fifo_w, O_WRONLY); //assuming ev. ok
	int rfd = open(fifo_r, O_RDONLY);

	write(wfd, "svcreq", 6);
	
	char buf[6] = { 0 };
	read(rfd, buf, 5);

	close(sfd);
	close(wfd);
	unlink(fifo_w);
	unlink(fifo_r);
	return strcmp(buf, "svcok") == 0;
}

int main(int argc, char* argv[]) {
	int n_thrs = DFL_THRS;
	for(int i = 0; i < argc; ++i) {
		if((strcmp("-t", argv[i]) == 0 || 
				strcmp("--threads", argv[i]) == 0) && i < argc - 1) {
			char* endptr = NULL;
			n_thrs = strtol(argv[i + 1], &endptr, 10);
			if(errno) {
				printf("unable to convert to integer (strtol): %s\n", strerror(errno));
				n_thrs = DFL_THRS;
			}
		}
	}

	printf("no of threads: %d\n", n_thrs);

	pthread_t* thrs = (pthread_t*) malloc(sizeof(pthread_t) * n_thrs);
	if(thrs == NULL) {
		puts("memory exhausted");
		return EXIT_FAILURE;
	}

	for(int i = 0; i < n_thrs; ++i) {
		printf("spawning %d-th thread\n", i);
		if(pthread_create(&thrs[i], NULL, PTHREAD_ROUTINE(askthr), PTHREAD_ARGUMENT(i)) < 0) {
			thrs[i] = 0xDEADBEEF;
			printf("unable to spawn thread %d-th -- (pthread_create): %s\n", i, strerror(errno));
		}
	}

	for(int i = 0; i < n_thrs; ++i) {
		if(thrs[i] != 0xDEADBEEF) {
			printf("waiting for %d-th thread...", i);
			fflush(stdout);
			int val;
			pthread_join(thrs[i], &val);
			if(val) printf(" received OK response\n");
			else printf("...\n");
		}
	}

	free(thrs);
	return EXIT_SUCCESS;
}

