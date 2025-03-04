#include <complex.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define BACKLOG 20

void checkErrno(void) {
    if (errno != 0) {  // we can check if errno is a non-zero value, errno always gets updated to the last error value
        fprintf(stderr, "[ERROR]: %s\n", strerror(errno));   // strerror converts errno values to human-readable for us!
    }
}

int main(void) {
    const char *node = "127.0.0.1";
    const char *service = "10250"; // use above 1024 till 65535, if they aren't already used by some other program

    struct addrinfo hints;
    struct addrinfo *servinfo;
    memset(&hints ,0, sizeof(hints)); // zero it
    hints.ai_family = AF_INET;        // AF_INET - IPv4 || AF_INET6 - IPv6
    hints.ai_socktype = SOCK_STREAM;


    // 0.
    int status = getaddrinfo(node, service, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status)); // convert getaddrinfo error to string
        checkErrno();
    } else {
        printf("[SERVER] Fetching address information successful.\n");
    }

    // 1.
    int listen_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (listen_fd == -1) {
        fprintf(stderr, "socket() error: %s\n", gai_strerror(listen_fd));
        checkErrno();
    } else {
        printf("[SERVER] Listening socket created.\n");
    }

    // 2.
    int bind_rv = bind(listen_fd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (bind_rv == -1) {
        fprintf(stderr, "bind() error: %s\n", gai_strerror(bind_rv));
        checkErrno();
    } else {
        printf("[SERVER] Succesfully bound to port: %s\n", service);
    }

    // 3.
    bool listening = false;
    int listen_rv = listen(listen_fd,BACKLOG); // 20 - backlog specifies how many connections are allowed on the incomming queue
    if (listen_rv == -1) {
        fprintf(stderr, "listen() error: %s\n", gai_strerror(bind_rv));
        checkErrno();
    } else {
        printf("[SERVER] Listening on: %s:%s\n", node, service);
        listening = true;
    }

    socklen_t addr_size;
    struct sockaddr_storage their_addr;
    addr_size = sizeof their_addr;

    // 4.
    char buffer[512];
    int bytes_received;
    int bytes_sent;
    int comms_fd;
    bool transmittingData = false;
    while (listening) {
        // 4.
        comms_fd = accept(listen_fd, (struct sockaddr *)&their_addr, &addr_size);
        if (comms_fd == -1) {
            fprintf(stderr, "accept() error: %s\n", gai_strerror(bind_rv));
            checkErrno();
        } else {
            printf("[SERVER] Accepted connection.\n");
            transmittingData = true;
        }

        while(transmittingData) {
            // 5.
            bytes_received = recv(comms_fd, buffer, sizeof(buffer), 0);
            if (bytes_received == -1) {
                fprintf(stderr, "recv() error: %s\n", gai_strerror(bind_rv));
                checkErrno();
            } else if (bytes_received == 0) {
                printf("[SERVER] Remote side has closed the connection\n");
                break;
            } else {
                printf("[SERVER] %s", buffer);
            }

            // 5.
            bytes_sent = send(comms_fd, buffer, strlen(buffer) + 1, 0);
            if (bytes_sent == -1) {
                fprintf(stderr, "send() error: %s\n", gai_strerror(bytes_sent));
                checkErrno();
            }

            // 6.
            // clear buffer, get ready to recv() next msg
            for (int i = 0; i != sizeof(buffer); i++) {
                buffer[i] = '\0';
            }
        }
    }

    close(comms_fd);
    close(listen_fd);
    freeaddrinfo(servinfo);
    return 0;
}
