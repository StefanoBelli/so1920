#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
	int fd = open("F", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		puts("impossibile aprire il file");
		return 0;
	}

	pid_t p = fork();
	if(p == 0)
		write(fd, "messaggio 0", 11); 
	else if(p == -1) {
		close(fd);
		return 0;
	} else {	
		int status;
		wait(&status);

		lseek(fd, 0, 0);

		char buffer[12] = { 0 };
		read(fd, buffer, 11);

		puts(buffer);
		close(fd);
	}

	return 0;
}
