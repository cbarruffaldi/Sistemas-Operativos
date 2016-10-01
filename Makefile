
#Agregar -Wall y -Wextra eventualmente

FLAGS= -g -Wall
CLIENT=client.c client_commands.c marshalling.c client_marshalling.c
CLIENT_BIN = client.bin
SERVER=server.c marshalling.c server_marshalling.c query.c
SERVER_BIN=server.bin
DATABASE=database.c
DATABASE_BIN=database.bin


sockets:
	gcc $(FLAGS) $(DATABASE) sockets_IPC.c -l sqlite3 -l pthread -o $(DATABASE_BIN)
	gcc $(FLAGS) $(CLIENT) sockets_IPC.c -o $(CLIENT_BIN)
	gcc $(FLAGS) $(SERVER) sockets_IPC.c -l pthread -o $(SERVER_BIN)

fifo:
	rm -f -v fifo_server fifo_peer_*
	gcc $(FLAGS) $(DATABASE) fifo_IPC.c -l sqlite3 -l pthread -o $(DATABASE_BIN)
	gcc $(FLAGS) $(CLIENT) fifo_IPC.c -o $(CLIENT_BIN)
	gcc $(FLAGS) $(SERVER) fifo_IPC.c -l pthread -o $(SERVER_BIN)

clean:
	rm -v *.bin
	rm -f -v fifo_server /tmp/fifo_peer_*
