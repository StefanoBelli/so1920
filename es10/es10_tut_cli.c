#ifndef DFL_THRS
#define DFL_THRS 2
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

#define log_thread_error(msg) \
	printf("thread %ld - %s: %s\n", syscall(SYS_gettid), msg, strerror(errno))

void* wronly(void* unused) {
	int servfd = open("server_fifo", O_WRONLY);
	if(servfd == -1) {
		log_thread_error("open");
		return NULL;
	}

	int fd = -1;
	char fifoname[NAME_MAX] = { 0 };

	for(int i = 0; i < INT_MAX; ++i) {
		snprintf(fifoname, NAME_MAX, "client_fifo_%d", i);
		int check = mkfifo(fifoname, O_CREAT | O_EXCL | S_IRUSR | S_IWUSR);
		if(check >= 0) {
			fd = open(fifoname, O_RDWR);
			break;
		}
		//memset(fifoname, 0, NAME_MAX);
	}

	if(fd == -1) {
		log_thread_error("open");
		close(servfd);
		return NULL;
	}

	char sendname[NAME_MAX + 1] = { 0 };
	snprintf(sendname, NAME_MAX + 1, "%s\n", fifoname);

	if(write(servfd, sendname, strlen(sendname)) == -1) {
		log_thread_error("write");
		close(servfd);
		close(fd);
		return NULL;
	}

	if(write(fd, "svcreq", sizeof("svcreq")) == -1) {
		log_thread_error("write");
		close(servfd);
		close(fd);
		return NULL;
	}

	sleep(1); //soluzione di merda, andrebbero usate due fifo diverse

	char buf[sizeof("svcok")] = { 0 };
	if(read(fd, buf, sizeof("svcok")) == -1) {
		log_thread_error("read");
		close(servfd);
		close(fd);
		return NULL;
	}

	int res = strcmp(buf, "svcok") == 0;

	close(fd);
	close(servfd);
	unlink(fifoname);

	return (void*)res;
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
		if(pthread_create(&thrs[i], NULL, wronly, NULL) < 0) {
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
		}
	}

	free(thrs);
	return EXIT_SUCCESS;
}

