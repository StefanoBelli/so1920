#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main() {
	int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sd < 0) {
		perror("socket");
		return EXIT_FAILURE;
	}

	struct sockaddr_in dst = { 0 };
	dst.sin_family = AF_INET;
	dst.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	dst.sin_port = htons(9999);

	struct sockaddr unused_src;
	socklen_t unused_srclen;
	
	socklen_t dstlen = sizeof(struct sockaddr_in);

	char buf[256];
	while(1) {
		memset(buf, 0, 256);
		fgets(buf, 255, stdin);
		sendto(sd, (const void*) buf, strlen(buf) + 1, 0, (struct sockaddr*) &dst, dstlen);
		memset(buf, 0, 256);
		recvfrom(sd, (void*) buf, 255, 0, &unused_src, &unused_srclen);
		puts(buf);
	}

	close(sd);
	return EXIT_SUCCESS;
}
