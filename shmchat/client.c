#if (defined(UPPER) && defined(LOWER)) || (!defined(UPPER) && !defined(LOWER))
#error define upper or lower
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <semaphore.h>

#define MEMKEY 0xABCD1234
#define BUF_SIZE 256

#define error(what, msg) \
	if(what < 0) { \
		perror(msg); \
		exit(EXIT_FAILURE); \
	}

int main() {
	int shmid = shmget(MEMKEY, 0, 0);
	error(shmid, "shmget");

	void* shared_memory = shmat(shmid, NULL, 0);
	char* text_region = shared_memory + sizeof(sem_t) + sizeof(sem_t);

	do {
//		(* funziona allo stesso modo, prima non andava per errore di init del semaforo, vedi commit precedenti *)

#ifdef UPPER
		error(sem_wait((sem_t*)shared_memory + sizeof(sem_t)), "sem_wait");
		printf("ack\n");
#endif

		printf("text: ");
		fflush(stdout);
		char tmpbuf[BUF_SIZE] = { 0 };
		fgets(text_region, BUF_SIZE, stdin);
		error(sem_post((sem_t*) shared_memory), "sem_post");
		memcpy(tmpbuf, text_region, BUF_SIZE);

#ifdef LOWER
		error(sem_wait((sem_t*)shared_memory + sizeof(sem_t)), "sem_wait");
		printf("ack\n");
#endif

		if(strcmp(tmpbuf, "bye\n") == 0) break;
	} while(1);

	error(shmdt(shared_memory), "shmdt");
	return EXIT_SUCCESS;
}
