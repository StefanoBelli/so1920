#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>

int main() {
	int fds[2];
	socketpair(AF_UNIX, SOCK_DGRAM, 0, fds);

	pid_t p = fork();
	if(p == 0) {
		close(fds[0]);

		char buf[256] = { 0 };
		recvfrom(fds[1], buf, 255, 0, NULL, 0);
		printf("child: %s\n", buf);
		sendto(fds[1], "ACK", 3, 0, NULL, 0);

		close(fds[1]);
	} else {
		char recvb[4] = { 0 };
		close(fds[1]);
		sendto(fds[0], "kiao", 4, 0, NULL, 0);
		recvfrom(fds[0], recvb, 3, 0, NULL, 0);
		close(fds[0]);
		wait(NULL);
		printf("parent: %s\n", recvb);
	}
	
	return EXIT_SUCCESS;
}
