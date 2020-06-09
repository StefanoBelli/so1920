#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>

#define ARGCMP(shrt, extd, arg) \
	(strcmp(shrt, arg) == 0 || strcmp(extd, arg) == 0)

#define VERBOSE if(verbose)

int main(int argc, char** argv) {
	int verbose = 0;
	for(int i = 0; i < argc; ++i) {
		if(ARGCMP("-v", "--verbose", argv[i]))
			verbose = 1;
	}

	int sd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sd < 0) {
		perror("socket");
		return EXIT_FAILURE;
	}

	struct sockaddr_un addr = { 0 };
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, "echoback.sock");
	if(connect(sd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) < 0) {
		perror("connect");
		close(sd);
		return EXIT_FAILURE;
	}
	
	VERBOSE puts("connected");

	char inout_buf[256];
	do {
		memset(inout_buf, 0, 256);
		fgets(inout_buf, 255, stdin);

		VERBOSE puts("sending");

		if(send(sd, inout_buf, strlen(inout_buf) + 1, MSG_NOSIGNAL) < 0) {
			if(errno == EPIPE) puts("server closed connection");
			else perror("send");

			break;
		}

		memset(inout_buf, 0, 256);
		int res = recv(sd, inout_buf, 256, 0);	
		if(res < 0) {
			perror("recv");
			break;
		} else if(res == 0) {
			puts("server closed connection");
			break;
		}

		VERBOSE { printf("gotans\n:"); fflush(stdout); }
		printf("%s\n",inout_buf);
		
	} while(1);

	close(sd);
	return EXIT_SUCCESS;
}
