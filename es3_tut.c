#include <Windows.h>
#include <stdio.h>

#define INPUT_MAX 256

int main(int argc, char** argv) {
	if(argc > 1) {
		for (int i = 1; i < argc; ++i)
			printf("%s ", argv[i]);
		return 0;
	}

	char str[INPUT_MAX + 14];
	
	memcpy(str, ".\\es3_tut.exe ", 14);
	fgets(str + 14, INPUT_MAX, stdin);

	puts(str);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	BOOL is_created = CreateProcess(
			".\\es3_tut.exe", 
			str, 
			NULL, 
			NULL, 
			FALSE, 
			NORMAL_PRIORITY_CLASS, 
			NULL, 
			NULL, 
			&si,
			&pi);

	if(!is_created) {
		printf("il nuovo processo non può essere creato\n");
		return 1;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	return 0;
}
