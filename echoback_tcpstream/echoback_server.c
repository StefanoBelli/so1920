#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
	int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sd < 0) {
		perror("socket");
		return EXIT_FAILURE;
	}

	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	//hardware to network byte ordering (long)
	addr.sin_port = htons(9999);				//hardware to netowrk byte ordering (short)
	if(bind(sd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		close(sd);
		return EXIT_FAILURE;
	}

	if(listen(sd, 2) < 0) {
		perror("listen");
		close(sd);
		return EXIT_FAILURE;
	}

	while(1) {
		struct sockaddr_in addr = { 0 };
		socklen_t len = sizeof(struct sockaddr_in);

		int newsd = accept(sd, (struct sockaddr*) &addr, &len);
		if(newsd > 0) {
			struct sockaddr_in local_addr = { 0 };
			socklen_t local_addrlen = sizeof(struct sockaddr_in);

			getsockname(newsd, (struct sockaddr*) &local_addr, &local_addrlen);
			printf("received connection: %s port: %d (src=%s,srcport=%d)\n", 
					inet_ntoa(addr.sin_addr), 
					ntohs(addr.sin_port), 
					inet_ntoa(local_addr.sin_addr), 
					ntohs(local_addr.sin_port));
			char buf[256] = { 0 };
			while(recv(newsd, buf, 255, 0) > 0) {
				send(newsd, buf, strlen(buf) + 1, MSG_NOSIGNAL);
				memset(buf, 0, 256);
			}
			close(newsd);
		}
	}

	close(sd);
	return EXIT_SUCCESS;
}
