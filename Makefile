FLAGS=-Wall -Wextra -g

all:
	gcc $(FLAGS) client.c fifo_IPC.c -o client.bin
	gcc $(FLAGS) server.c fifo_IPC.c -o server.bin

clean:
	rm -v *.bin
