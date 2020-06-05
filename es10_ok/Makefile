GCC_WFLAGS=-W -Wextra -Wall -Wshadow -pthread

all:
	gcc -o server server.c $(GCC_WFLAGS)
	gcc -o client client.c $(GCC_WFLAGS)
	
clean:
	rm -rfv client_fifo* server_fifo

