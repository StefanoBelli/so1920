#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define error_exit(res, pmsg, ...) \
	if(res < 0) { \
		perror(pmsg); \
		__VA_ARGS__ \
		exit(EXIT_FAILURE); \
	}

int main() {
	int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	error_exit(sd, "socket");

	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(9999);

	int res = connect(sd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
	error_exit(res, "connect", close(sd););
	
	char buf[256];
	while(1) {
		memset(buf, 0, 256);
		fgets(buf, 255, stdin);
		
		if(send(sd, buf, strlen(buf) + 1, MSG_NOSIGNAL) < 0) {
			if(errno == EPIPE)
				puts("server closed connection");
			else
				perror("send");
			break;
		}

		if(recv(sd, buf, 255, 0) <= 0) {
			if(errno)
				perror("recv");
			else
				puts("server closed connection");
			break;
		}
		puts(buf);
	}

	close(sd);
	return EXIT_SUCCESS;
}
