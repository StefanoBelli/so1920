#ifndef N_THREADS
#define N_THREADS 4
#endif

#ifndef MAX_INPUT
#define MAX_INPUT 256
#endif

#include <Windows.h>
#include <stdio.h>

#define TSR(f) ((LPTHREAD_START_ROUTINE)f)

typedef struct __THREAD_PARAMS {
	int threadIndex;
	int maxThreads;
	char message[MAX_INPUT];
	HANDLE* nextThread;
} THREAD_PARAMS;

void ThreadAskSend(__in THREAD_PARAMS* params) {
	if (params->message[0])
		printf("thr %d -- prev thread (%d) says: %s", 
			params->threadIndex, 
			params->threadIndex - 1, 
			params->message);

	if (params->threadIndex < params->maxThreads - 1) {
		printf("thr %d -- input: ", params->threadIndex);
		fflush(stdin);

		fgets((params + 1)->message, MAX_INPUT, stdin);

		puts("");

		ResumeThread(*params->nextThread);
	}
	else
		printf("thr %d -- terminated\n", params->threadIndex);
}

int main() {
	HANDLE threads[N_THREADS];
	THREAD_PARAMS p[N_THREADS];

	for (int i = 0; i < N_THREADS; ++i) {
		p[i].message[0] = 0;
		p[i].threadIndex = i;
		p[i].nextThread = &threads[i + 1];
		p[i].maxThreads = N_THREADS;

		threads[i] = CreateThread(NULL,
			0,
			TSR(ThreadAskSend),
			(void*)&p[i],
			CREATE_SUSPENDED,
			NULL);
	}
	
	ResumeThread(threads[0]);

	for (int i = 0; i < N_THREADS; ++i)
		if (WaitForSingleObject(threads[i], INFINITE) == WAIT_FAILED)
			puts("main thr: WaitForSingleObject failed (WAIT_FAILED)");

	return 0;
}