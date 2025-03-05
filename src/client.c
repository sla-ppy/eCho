#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(void) {
    const char *server_addr = "127.0.0.1";
    const char *server_port = "10250"; // use above 1024 till 65535, if they aren't already used by some other program

    struct addrinfo hints;
    memset(&hints ,0, sizeof(hints)); // zero it
    hints.ai_family = AF_INET;        // AF_INET - IPv4 || AF_INET6 - IPv6
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *servinfo;

    // 0.
    int rc = getaddrinfo(server_addr, server_port, &hints, &servinfo);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo() error: %s\n", strerror(errno));
        return 1;
    } else {
        printf("[CLIENT] Fetching address information successful.\n");
    }

    // 1.
    int client_sfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (client_sfd == -1) {
        fprintf(stderr, "socket() error: %s\n", strerror(errno));
        return 1;
    } else {
        printf("[CLIENT] Connecting socket created.\n");
    }

    // 2.
    rc = connect(client_sfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (rc == -1) {
        fprintf(stderr, "connect() error: %s\n", strerror(errno));
    } else {
        printf("[CLIENT] Succesfully connected to %s:%s\n", server_addr, server_port);
    }

    char buffer[512];
    int bytes_sent;
    int bytes_received = 1;
    while (true) {
        // 5. check if server wants to send anything first
        bytes_received = recv(client_sfd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            fprintf(stderr, "recv() error during transcieving: %s\n", strerror(errno));
        } else if (bytes_received == 0) {
            printf("[SERVER] Remote side has closed the connection\n");
            break;
        } else {
            printf("[SERVER] %s", buffer);
        }

        printf("Enter message: ");
        fgets(buffer, 512, stdin);  // fgets() adds a terminating \0 at the end of the string

        // 6. send any message we want
        bytes_sent = send(client_sfd, buffer, strlen(buffer)+1, 0); // strlen() by default excludes \0, so we +1
        if (bytes_sent == -1) {
            fprintf(stderr, "send() error during transcieving: %s\n", strerror(errno));
        }
    }

    close(client_sfd);
    freeaddrinfo(servinfo);
    return 0;
}
