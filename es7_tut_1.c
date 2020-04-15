#ifndef CPU_CORES
#error define CPU_CORES (-DCPU_CORES=N)
#endif

#include <stdio.h>
#include <Windows.h>

VOID routine(DWORD* mask) {
    HANDLE thisThread = GetCurrentThread();
    int tid = GetThreadId(thisThread);

#ifndef DO_NOT_SET_AFFINITY
    printf("thread %d setting affinity mask: %d\n", tid, *mask);
    if(SetThreadAffinityMask(thisThread, *mask) == 0)
        printf("SetThreadAffinityMask failure");
#endif

    printf("thread (%d)... ", tid);
    fflush(stdout);

    int i = 1000000000;
    while(i >= 0) --i;

    puts("FINISHED");
    CloseHandle(thisThread);
}

int main() {

    HANDLE thisProcess = GetCurrentProcess();

    DWORD processCpuMask;
    DWORD systemCpuMask;
    GetProcessAffinityMask(thisProcess, &processCpuMask, &systemCpuMask);
    CloseHandle(thisProcess);

    printf("process affinity mask: %d\n", processCpuMask);

    for(int i = 0; i < CPU_CORES; ++i) {
        DWORD cpu_mask = 1 << i;

        HANDLE thr = CreateThread(
            NULL, 0, (LPTHREAD_START_ROUTINE)routine, (LPVOID)&cpu_mask, 0, NULL);
        if(thr == NULL) {
            puts("CreateThread error");
            exit(EXIT_FAILURE);
        }

        WaitForSingleObject(thr, INFINITE);
        CloseHandle(thr);
    }

    return EXIT_SUCCESS;
}