## Info:

eCho is a TCP server/client implementation which was created by following [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split-wide/)\
The default address is set to the loopback address, and I advise against using a different address to avoid.

By using the program you accept the risks which come with opening your ports, which can be a security risk.\
Make sure you close the server once you are done trying it out and do not it for an extended period of time!

## Server usage:

1. Compile using:
```
clang src/server.c -o server -Wall -Wextra -pedantic -std=gnu99`
```
2. Run by:
```
./server
```
3. The server automatically starts listening on port 10250 and is ready to receive the client's messages

---

## Client usage:
0. Make sure the Server is already listening without any problems, otherwise Client will fail to connect.

1. Compile using:
```
clang src/client.c -o client -Wall -Wextra -pedantic -std=gnu99
```

2: Run by:
```
./client
```

3. The client can now be used to transmit shorter messages
4. Once finished, you can close Client by using Ctrl+C and the Server will automatically close.
