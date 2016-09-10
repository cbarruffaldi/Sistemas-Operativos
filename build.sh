rm -f -v fifo_server fifo_cid client_n_*
gcc client.c -o client
gcc server.c -o server
gcc fifo_build.c -o fifo_build
./fifo_build
