#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <stdio.h>

int main() {

#ifdef _WIN32
	HANDLE file = CreateFile(TEXT("file.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		puts("failure");
		return 1;
	}

	HANDLE map = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (map == INVALID_HANDLE_VALUE) {
		CloseHandle(file);
		puts("map failure");
		return 2;
	}

	CHAR* addrStart = (CHAR*) MapViewOfFile(map, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	CloseHandle(file);			// libera handles ASAP
	CloseHandle(map);

	if (addrStart == NULL) {
		puts("unable to map");
		return 3;
	}

	*addrStart = 's';
#else
	int fd = open("file.txt", O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	struct stat stat_buf;
	fstat(fd, &stat_buf);


	char* addrStart = (char*)mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (addrStart == MAP_FAILED) {
		perror("mmap");
		return 2;
	}
#endif

	char ch;
	while ((ch=*(addrStart++)))
		putc(ch, stdout);

#ifdef _WIN32
	UnmapViewOfFile(addrStart);
#else
	munmap(addrStart, stat_buf.st_size);
#endif

	return 0;
}