### first
install libuv
cd into libuv and read install file

### how to build
gcc -c server.c -o build/server.o
gcc -pthread -o server build/server.o /usr/local/lib/libuv.a

gcc -c client.c -o build/client.o
gcc -pthread -o client build/client.o /usr/local/lib/libuv.a

### how to run
./server
./client

### result
client send data to server,server echo back