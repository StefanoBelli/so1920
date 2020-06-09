#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define CHUNK_SIZE 4096

int main() {
	// 1) lookup
	struct hostent* host_entity = gethostbyname("yahoo.com");
	if(host_entity == NULL) {
		puts("unable to lookup yahoo.com");
		return EXIT_FAILURE;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr = *(struct in_addr*) host_entity->h_addr;
	addr.sin_port = htons(80);

	// 2) socket setup
	int sd = socket(AF_INET, SOCK_STREAM, 0); //IPPROTO_TCP
	if(sd < 0) {
		perror("socket");
		return EXIT_FAILURE;
	}

	// 3) connect
	if(connect(sd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0) {
		perror("connect");
		close(sd);
		return EXIT_FAILURE;
	}

	// 4) send HTTP request & get HTTP response
	char http_get_req[] = "GET /index.html HTTP/1.1\r\nUser-Agent: test-http-client\r\nHost: yahoo.com\r\n\r\n";
	if(send(sd, http_get_req, sizeof(http_get_req), 0) < 0) {
		perror("send");
		close(sd);
		return EXIT_FAILURE;
	}

	int reason;
	char buf[CHUNK_SIZE] = { 0 };
	while((reason = recv(sd, buf, CHUNK_SIZE, 0)) > 0) {
		printf("%s",buf);
		memset(buf, 0, CHUNK_SIZE);
	}

	if(reason < 0) {
		perror("recv");
		close(sd);
		return EXIT_FAILURE;
	}

	close(sd);
	return EXIT_SUCCESS;
}
