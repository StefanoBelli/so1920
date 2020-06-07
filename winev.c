#define TEST_SEGV

#include <Windows.h>
#include <signal.h>
#include <stdio.h>

BOOL WINAPI HandleRequest(DWORD ev) { //New thread
	if (ev != CTRL_C_EVENT)
		return FALSE; //let another handler take control

	for (int i = 0; i < 50; ++i) {
		printf("HANDLER: calculate %d\n", i);
		Sleep(500);
	}

	return TRUE;
}

void SegmentViolationInterruptHandler(int sig_unused) {
	puts("SEGV, exiting");
	ExitProcess(1);
}

int main() {
	SetConsoleCtrlHandler(HandleRequest, TRUE);
	signal(SIGSEGV, SegmentViolationInterruptHandler);

	while (1) {
		printf("main thread calculations...\n");
		Sleep(100);

#ifdef TEST_SEGV
		int *a = (int*)0;
		*a = 1;
#endif
	}
}