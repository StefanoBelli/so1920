#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
	int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sd < 0) {
		perror("socket");
		return EXIT_FAILURE;
	}

	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9999);

	if(bind(sd, (struct sockaddr*) &addr, sizeof(struct sockaddr)) < 0) {
		perror("bind");
		close(sd);
		return EXIT_FAILURE;
	}

	struct sockaddr_in local_addr = { 0 };
	socklen_t len = sizeof(struct sockaddr_in);
	getsockname(sd, (struct sockaddr*)&local_addr, &len);

	char buf[256];
	while(1) {
		memset(buf, 0, 256);

		struct sockaddr_in fromaddr = { 0 };
		socklen_t fromaddrlen = sizeof(struct sockaddr_in);
		recvfrom(sd, (void*) buf, 255, 0, (struct sockaddr*) &fromaddr, &fromaddrlen); 
		printf("datagram from %s (now dst port %d) (srcaddr=%s, srcport=%d)\n", 
				inet_ntoa(fromaddr.sin_addr), 
				ntohs(fromaddr.sin_port),
				inet_ntoa(local_addr.sin_addr),
				ntohs(local_addr.sin_port));
		sendto(sd, (void*) buf, strlen(buf) + 1, 0, (struct sockaddr*) &fromaddr, fromaddrlen);
	}

	close(sd);
	return EXIT_SUCCESS;
}
