GCC_COMMON_FLAGS = -pthread -Wall -Wextra -W -Wshadow 

all:
	gcc -o server server.c $(GCC_COMMON_FLAGS)
	gcc -o client client.c -DLOWER $(GCC_COMMON_FLAGS)
