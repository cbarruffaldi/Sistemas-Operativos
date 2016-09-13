FLAGS=-Wall -Wextra -g

all:
	rm -f -v fifo_server fifo-peer-*
	gcc $(FLAGS) client.c fifo_IPC.c -o client.bin
	gcc $(FLAGS) server.c fifo_IPC.c -o server.bin

clean:
	rm -v *.bin
	rm -f -v fifo_server fifo-peer-*
