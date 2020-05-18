#define F "file.txt"
#define N 5
#define W 6

#include <Windows.h>
#include <stdio.h>
#include <string.h>

#define ErrorString(errn, target)\
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |	\
					FORMAT_MESSAGE_FROM_SYSTEM | \
					FORMAT_MESSAGE_IGNORE_INSERTS, \
					NULL, errn, \
					MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), \
					target, 0, NULL)

#define ErrorDie(res) if(!res) { \
    char* err; \
    ErrorString(GetLastError(), err); \
	printf("[%d] " #res ": %s\n", GetProcessId(GetCurrentProcess()), err); \
    LocalFree(err); \
	return EXIT_FAILURE; \
}

void DoWork(HANDLE fileHandle, int procIndex) {

}

int main(int argc, char** argv) {
	HANDLE fileHnd = NULL;
	int procIdx = -1;
	for (int i = 0; i < argc; ++i) {
		if (strcmp("-FileHandle", argv[i]) == 0) {
			char* end = NULL;
			fileHnd = (HANDLE)strtol(argv[i + 1], &end, 10);
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
			CREATE_ALWAYS | TRUNCATE_EXISTING,
			0,
			NULL);

		ErrorDie(file);
		CloseHandle(file);
	}
	else {
		DoWork(fileHnd, procIdx);
		CloseHandle(fileHnd);
	}

	return 0;
}