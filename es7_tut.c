#ifndef CPU_CORES
#error define CPU_CORES (-DCPU_CORES=N)
#endif

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <syscall.h>
#include <sys/syscall.h>

#define die_iferr(r,msg) \
    if(r != 0) { \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }

#define gettid() syscall(SYS_gettid)

void* routine(cpu_set_t* mask) {
    pthread_t this_thread = pthread_self();

#ifndef DO_NOT_SET_AFFINITY
    die_iferr(pthread_setaffinity_np(this_thread, sizeof(cpu_set_t), mask), "pthread_setaffinity_np");
#endif

    cpu_set_t current_mask;
    die_iferr(pthread_getaffinity_np(this_thread, sizeof(cpu_set_t), &current_mask), "pthread_getaffinity_np");

    printf("thread (%ld) using ", gettid());
    for(int i = 0; i < CPU_CORES; ++i)
        if(CPU_ISSET(i, &current_mask))
            printf("cpu %d, ", i);
    fflush(stdout);

    int i = 1000000000;
    while(i >= 0) --i;

    puts("FINISHED");
}

int main() {
    for(int i = 0; i < CPU_CORES; ++i) {
        cpu_set_t cpu_mask;
        CPU_ZERO(&cpu_mask);
        CPU_SET(i, &cpu_mask);

        pthread_t t;
        die_iferr(pthread_create(&t, NULL, (void*(*)(void*))routine, (void*)&cpu_mask), "pthread_create");
        die_iferr(pthread_join(t, NULL), "pthread_join");
    }

    return EXIT_SUCCESS;
}