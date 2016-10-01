
#Agregar -Wall y -WExtra eventualmente

FLAGS= -g
CLIENT=client.c client_commands.c client_marshalling.c
CLIENT_BIN = client.bin
SERVER=server.c server_marshalling.c query.c
SERVER_BIN=server.bin
DATABASE=database.c
DATABASE_BIN=database.bin
LOG_BIN=log.bin
LOG=log.c


sockets:
	gcc $(FLAGS) $(DATABASE) sockets_IPC.c -l sqlite3 -l pthread -o $(DATABASE_BIN)
	gcc $(FLAGS) $(CLIENT) sockets_IPC.c -o $(CLIENT_BIN)
	gcc $(FLAGS) $(SERVER) sockets_IPC.c -lrt -l pthread -lrt -o $(SERVER_BIN)
	gcc $(FLAGS) $(LOG) -lrt -o $(LOG_BIN)
fifo:
	rm -f -v fifo_server fifo_peer_*
	gcc $(FLAGS) $(DATABASE) fifo_IPC.c -l sqlite3 -l pthread -o $(DATABASE_BIN)
	gcc $(FLAGS) $(CLIENT) fifo_IPC.c -o $(CLIENT_BIN)
	gcc $(FLAGS) $(SERVER) fifo_IPC.c -lrt -l pthread -o $(SERVER_BIN)
	gcc $(FLAGS) $(LOG) -lrt -o $(LOG_BIN)

clean:
	rm -v *.bin
	rm -f -v fifo_server /tmp/fifo_peer_*
