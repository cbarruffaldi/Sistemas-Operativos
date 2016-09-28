FLAGS=-Wall -Wextra -g
CLIENT=client.c
SERVER=server.c
DATABASE=database.c

sockets:
	gcc $(FLAGS) $(CLIENT) sockets_IPC.c -o client.bin
	gcc $(FLAGS) $(SERVER) sockets_IPC.c -l pthread -o server.bin
	gcc $(FLAGS) $(DATABASE) sockets_IPC.c -l sqlite3 -l pthread -o database.bin

fifo:
	rm -f -v fifo_server fifo_peer_*
	gcc $(FLAGS) $(CLIENT) fifo_IPC.c -o client.bin
	gcc $(FLAGS) $(SERVER) fifo_IPC.c -l pthread -o server.bin
	gcc $(FLAGS) $(DATABASE) fifo_IPC.c -l sqlite3 -l pthread -o database.bin

clean:
	rm -v *.bin
	rm -f -v fifo_server /tmp/fifo_peer_*
