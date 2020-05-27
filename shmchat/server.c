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
	int shmid = shmget(
			MEMKEY, sizeof(sem_t) + sizeof(sem_t) + BUF_SIZE, 
			IPC_CREAT | IPC_EXCL | 0666
			);
	error(shmid, "shmget");

	void* shared_memory = shmat(shmid, NULL, 0);
	error(sem_init((sem_t*)shared_memory, 1, 0), "sem_init");
	error(sem_init((sem_t*)shared_memory + sizeof(sem_t), 1, 0), "sem_init"); //ed è proprio qui che vorrei scrivere un bestemmione (vedi commit precedente)

	char* text_region = shared_memory + sizeof(sem_t) + sizeof(sem_t);

	while(strcmp(text_region, "bye\n") != 0) {
		memset(text_region, 0, BUF_SIZE);
		error(sem_post((sem_t*)shared_memory + sizeof(sem_t)), "sem_post");
		error(sem_wait((sem_t*)shared_memory), "sem_wait");
		printf("received: %s\n", text_region);
	}

	error(sem_destroy((sem_t*)shared_memory), "sem_destroy");
	error(sem_destroy((sem_t*)shared_memory + sizeof(sem_t)), "sem_destroy");
	error(shmdt(shared_memory), "shmdt");
	error(shmctl(shmid, IPC_RMID, 0), "shmctl");
	return EXIT_SUCCESS;
}
