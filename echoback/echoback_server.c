#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define ARGCMP(shrt, extd, arg) \
	(strcmp(shrt, arg) == 0 || strcmp(extd, arg) == 0)

#define VERBOSE if(verbose)

int sd = -1;

void sigint(int sig) {
	close(sd);
	unlink("echoback.sock");
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
	int verbose = 0;

	for(int i = 0; i < argc; ++i) {
		if(ARGCMP("-v", "--verbose", argv[i])) 
			verbose = 1;
	}

	sd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sd < 0) {
		perror("socket");
		return EXIT_FAILURE;
	}

	sigset_t set;
	sigfillset(&set);

	if(sigprocmask(SIG_BLOCK, &set, NULL) < 0) { /* begin protected area */
		perror("sigprocmask");
		close(sd);
		unlink("echoback.sock");
		return EXIT_FAILURE;
	}

	struct sockaddr_un addr = { 0 };
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, "echoback.sock");
	
	if(bind(sd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) < 0) {
		perror("bind");
		close(sd);
		unlink("echoback.sock");
		return EXIT_FAILURE;
	}

	struct sigaction act = { 0 };
	act.sa_handler = sigint;
	act.sa_mask = set;
	if(sigaction(SIGINT, &act, NULL) < 0) {
		perror("sigaction");
		close(sd);
		unlink("echoback.sock");
		return EXIT_FAILURE;
	}

	if(sigprocmask(SIG_UNBLOCK, &set, NULL) < 0) { /* protected area */
		perror("sigprocmask(SIG_UNBLOCK)");
		close(sd);
		unlink("echoback.sock");
		return EXIT_FAILURE;
	}
	
	if(listen(sd, 0) < 0) {
		perror("listen");
		close(sd);
		unlink("echoback.sock");
		return EXIT_FAILURE;
	}

	while(1) {
		struct sockaddr newaddr = { 0 };
		socklen_t newaddrlen;

		VERBOSE puts("waiting for new connection...");
		
		int newfd = accept(sd, &newaddr, &newaddrlen);
		if(newfd < 0) {
			perror("accept");
			close(sd);
			unlink("echoback.sock");
			return EXIT_FAILURE;
		}

		VERBOSE puts("got it, echoing back...");

		char buf[256] = { 0 };
		while(recv(newfd, buf, 255, 0) > 0) {
			if(send(newfd, buf, strlen(buf) + 1, 0))
				VERBOSE puts("sent back");
			memset(buf, 0, 255);
		}

		close(newfd);
	}

	close(sd);
	unlink("echoback.sock");
	return EXIT_SUCCESS;
}
