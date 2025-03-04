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

    bool sendingData = false;
    // 2.
    int connect_rv = connect(client_fd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (connect_rv == -1) {
        fprintf(stderr, "connect() error: %s\n", gai_strerror(connect_rv));
        checkErrno();
    } else {
        printf("[CLIENT] Succesfully connected to %s:%s\n", node, service);
        sendingData = true;
    }

    char buffer[512];
    int bytes_sent;
    int bytes_received;
    while (sendingData) {
        printf("Enter message: ");
        fgets(buffer, 512, stdin);  // fgets() adds a terminating \0 at the end of the string

        bytes_sent = send(client_fd, buffer, strlen(buffer)+1, 0); // strlen() by default excludes \0, so we +1
        if (bytes_sent == -1) {
            fprintf(stderr, "send() error: %s\n", gai_strerror(bytes_sent));
            checkErrno();
        }

        bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            fprintf(stderr, "recv() error: %s\n", gai_strerror(client_fd));
            checkErrno();
        } else if (bytes_received == 0) {
            printf("[SERVER] Remote side has closed the connection\n");
        } else {
            printf("[SERVER] %s", buffer);
        }
    }

    close(client_fd);
    freeaddrinfo(servinfo);
    return 0;
}
