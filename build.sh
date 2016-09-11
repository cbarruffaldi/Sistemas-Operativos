rm -f -v fifo_server fifo_client_* client_n_*
gcc client.c -o client
#gcc persistent_client.c -o persistent_client
gcc server.c -o server
gcc fifo_build.c -o fifo_build
./fifo_build
