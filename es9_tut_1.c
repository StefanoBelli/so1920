#include <Windows.h>
#include <stdio.h>
#include <string.h>

#define F "file.txt"
#define N 5
#define W 6

#define ErrorDie(res) if(!res) { \
	HANDLE currentProcess = GetCurrentProcess(); \
	printf("[%d] " #res ": %d\n", GetProcessId(currentProcess), GetLastError()); \
    CloseHandle(currentProcess); \
	return EXIT_FAILURE; \
}

int DoWork(HANDLE fileHandle, int procIndex) {
	HANDLE currentProcess = GetCurrentProcess();

	if(LockFile(fileHandle, 0, 0, 0, 0) == 0)
		return 1;

	char buf[22] = { 0 };
	snprintf(buf, 22, "%d:%d\n", GetProcessId(currentProcess), procIndex);
	size_t len = strlen(buf);

	for(int i = 0; i < W; ++i)
		WriteFile(fileHandle, buf, len, NULL, NULL);

	if(UnlockFile(fileHandle, 0, 0, 0, 0) == 0)
		return 2;

	CloseHandle(fileHandle);
	CloseHandle(currentProcess);
	return 0;
}

int main(int argc, char** argv) {
	HANDLE fileHnd = NULL;
	int procIdx = -1;
	for (int i = 0; i < argc; ++i) {
		if (strcmp("-FileHandle", argv[i]) == 0) {
			char* end = NULL;
			fileHnd = (HANDLE)strtoll(argv[i + 1], &end, 10);
		}
		else if (strcmp("-ProcIdx", argv[i]) == 0) {
			char* end = NULL;
			procIdx = strtol(argv[i + 1], &end, 10);
		}
	}

	if (fileHnd == NULL && procIdx == -1) {
		SECURITY_ATTRIBUTES security;
		security.bInheritHandle = TRUE;
		security.nLength = sizeof(SECURITY_ATTRIBUTES);
		security.lpSecurityDescriptor = NULL;

		HANDLE file = CreateFileA(F,
			GENERIC_WRITE,
			0,
			&security,
			CREATE_ALWAYS,
			0,
			NULL);

		ErrorDie(file);

		HANDLE childs[N];
		for(int i = 0; i < N; ++i) {
			char cmdLine[MAX_PATH + 44];
			snprintf(cmdLine, MAX_PATH + 44, "%s -FileHandle %lld -ProcIdx %d", argv[0], (long long) file, i);

			STARTUPINFOA sinfo;
			memset(&sinfo, 0, sizeof(STARTUPINFOA));
			sinfo.cb = sizeof(STARTUPINFOA);

			PROCESS_INFORMATION pinfo;

			BOOL pRes = CreateProcess(
					argv[0],
					cmdLine,
					NULL,
					NULL,
					TRUE,
					0,
					NULL,
					NULL,
					&sinfo,
					&pinfo);

			ErrorDie(pRes);
			CloseHandle(pinfo.hThread);
			childs[i] = pinfo.hProcess;
		}
		
		CloseHandle(file);
		
		for(int i = 0; i < N; ++i) {
			DWORD exitCode;
			WaitForSingleObject(childs[i], INFINITE);
			GetExitCodeProcess(childs[i], &exitCode);
			CloseHandle(childs[i]);
			printf("il processo [index:] %d -- terminato con codice %d\n", i, exitCode);
		}
	}
	else {
		int code = DoWork(fileHnd, procIdx);
		CloseHandle(fileHnd);
		ExitProcess(code);
	}

	return 0;
}
