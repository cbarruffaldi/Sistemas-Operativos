FLAGS=-Wall -Wextra -g
CLIENT=client.c
SERVER=server.c

sockets:
	gcc $(FLAGS) $(CLIENT) sockets_IPC.c -o client.bin
	gcc $(FLAGS) $(SERVER) sockets_IPC.c -l pthread -o server.bin

fifo:
	rm -f -v fifo_server fifo_peer_*
	gcc $(FLAGS) $(CLIENT) fifo_IPC.c -o client.bin
	gcc $(FLAGS) $(SERVER) fifo_IPC.c -l pthread -o server.bin

clean:
	rm -v *.bin
	rm -f -v fifo_server /tmp/fifo_peer_*
