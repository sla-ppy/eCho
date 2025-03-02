#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/*
TCP Echo Server is a simple echo server written in Golang that works plainly over the TCP protocol.
The server listens to connection requests, prepends a preconfigured prefix
to the data it receives from a client and writes it back on the connection.
*/

int main(void) {
    // get local time in calendar format
    time_t t = time(NULL);

    // format local time in calendar to custom format
    char buff[70];
    if (strftime(buff, sizeof buff, "[%Y.%m.%d - %H:%M:%S]", localtime(&t)))
        fputs(buff, stdout);
    else
        puts("*ERR*: strftime() failed when formatting local time!");
    printf(" Server: Connection established");

    // int getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);
    // nodename => www.example.com
    // servname => port number
    // 127.0.0.1 => IPv4 loopback addr
    int getaddrinfo();

    // take port from user through terminal
    char port[5] = "";


    return 0;
}
