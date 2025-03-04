#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void checkErrno() {
    if (errno != 0) {  // we can check if errno is a non-zero value, errno always gets updated to the last error value
        fprintf(stderr, "[CLIENT]: %s\n", strerror(errno));   // strerror converts errno values to human-readable for us!
    }
}

int main(void) {
    const char *node = "127.0.0.1";
    const char *service = "10250"; // use above 1024 till 65535, if they aren't already used by some other program

    struct addrinfo hints;
    memset(&hints ,0, sizeof(hints)); // zero it
    hints.ai_family = AF_INET;        // AF_INET - IPv4 || AF_INET6 - IPv6
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *servinfo;

    // 0.
    int status = getaddrinfo(node, service, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status)); // convert getaddrinfo error to string
        checkErrno();
    } else {
        printf("[CLIENT] Fetching address information successful.\n");
    }

    // 1.
    int client_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (client_fd == -1) {
        fprintf(stderr, "socket() error: %s\n", gai_strerror(client_fd));
        checkErrno();
    } else {
        printf("[CLIENT] Connecting socket created.\n");
    }

    bool receivingData = false;
    // 2.
    int connect_rv = connect(client_fd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (connect_rv == -1) {
        fprintf(stderr, "connect() error: %s\n", gai_strerror(connect_rv));
        checkErrno();
    } else {
        printf("[CLIENT] Succesfully connected to %s:%s\n", node, service);
        receivingData = true;
    }

    char buf[512];
    int bytes_sent;
    while (receivingData) {
        printf("Enter message: ");
        fgets(buf, 512, stdin);

        bytes_sent = send(client_fd, buf, strlen(buf), 0);
        if (bytes_sent == -1) {
            fprintf(stderr, "send() error: %s\n", gai_strerror(bytes_sent));
            checkErrno();
        }
    }

    close(client_fd);
    freeaddrinfo(servinfo);
    return 0;
}
