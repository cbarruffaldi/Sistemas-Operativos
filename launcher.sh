rm -f $1 $2
./database.bin $2 &
./server.bin $1 $2 &
