rm -f -v fifo_server fifo_client_* client_n_*
make
#gcc persistent_client.c -o persistent_client
gcc fifo_build.c -o fifo_build
./fifo_build
