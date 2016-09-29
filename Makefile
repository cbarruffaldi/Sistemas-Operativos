
#Agregar -Wall y -WExtra eventualmente

FLAGS= -g
CLIENT=client.c client_commands.c client_marshalling.c
CLIENT_BIN = client.bin
SERVER=server.c server_marshalling.c
SERVER_BIN=server.bin
DATABASE=database.c

sockets:
	gcc $(FLAGS) $(DATABASE) sockets_IPC.c -l sqlite3 -l pthread -o database.bin

fifo:
	rm -f -v fifo_server fifo_peer_*
	gcc $(FLAGS) $(DATABASE) fifo_IPC.c -l sqlite3 -l pthread -o database.bin

clean:
	rm -v *.bin
	rm -f -v fifo_server /tmp/fifo_peer_*
