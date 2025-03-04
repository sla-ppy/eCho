## Info:

eCho is a TCP server/client implementation which was created by following [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split-wide/)\
The default address is set to the loopback address, I recommend not changing it, nor using this for anything other than for educational purposes.

## Server usage:

1. Compile using:
```
clang src/server.c -o server -Wall -Wextra -pedantic -std=gnu99`
```
2. Run by:
```
./server
```
3. The Server automatically starts listening on port 10250 and is ready to accept the Client joining.
4. If a Client joins the Server will enter transmission mode until we have a Client present.
5. If the Client aborts its connection, the Server continues listening, hence close the connection by using CTRL+C

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
4. Once finished, you can close Client by using CTRL+C and connection to the Server will end.
5. You can connect again as the Server continues listening, check that out too!
