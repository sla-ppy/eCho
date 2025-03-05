#include <complex.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define BACKLOG 20

int main(void) {
    const char *server_addr = "127.0.0.1";
    const char *server_port = "10250"; // use above 1024 till 65535, if they aren't already used by some other program

    struct addrinfo hints;
    struct addrinfo *servinfo;
    memset(&hints ,0, sizeof(hints)); // zero it
    hints.ai_family = AF_INET;        // AF_INET - IPv4 || AF_INET6 - IPv6
    hints.ai_socktype = SOCK_STREAM;

    // 0. fill servinfo
    int rc = getaddrinfo(server_addr, server_port, &hints, &servinfo);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo() error: %s\n", strerror(errno));
        return 1;
    } else {
        printf("[SERVER] Fetching address information successful.\n");
    }

    // 1. create server socket
    int server_sfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (server_sfd == -1) {
        fprintf(stderr, "socket() error: %s\n", strerror(errno));
        return 1;
    } else {
        printf("[SERVER] Listening socket created.\n");
    }

    // 2. bind server socket to server port
    rc = bind(server_sfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (rc == -1) {
        fprintf(stderr, "bind() error: %s\n", strerror(errno));
        return 1;
    } else {
        printf("[SERVER] Succesfully bound to port: %s\n", server_port);
    }

    // 3. start listening on port
    rc = listen(server_sfd,BACKLOG); // 20 - backlog specifies how many connections are allowed on the incomming queue
    if (rc == -1) {
        fprintf(stderr, "listen() error: %s\n", strerror(errno));
        return 1;
    } else {
        printf("[SERVER] Listening on: %s:%s\n", server_addr, server_port);
    }

    // client data
    socklen_t addr_size;
    struct sockaddr_storage client_addr;
    addr_size = sizeof client_addr;
    char ip_addr[INET_ADDRSTRLEN];
    int client_port;

    // server data
    int client_sfd;
    int bytes_received;
    int bytes_sent;
    char buffer[512];
    char intro_msg[512] = "Welcome to eCho! Yell at me so I can yell back to you!\n";
    while (true) {
        // 4. We need two loops here so the server doesn't close after losing its client, this way client can reconnect, or in the future multiple clients can connect
        client_sfd = accept(server_sfd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sfd == -1) {
            fprintf(stderr, "accept() error: %s\n", strerror(errno));
            continue;
        }

        // get client data
        getpeername(client_sfd, (struct sockaddr *)&client_addr, &addr_size);
        struct sockaddr_in *s = (struct sockaddr_in *)&client_addr;
        client_port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ip_addr, sizeof(ip_addr));
        printf("[SERVER] Accepted connection: %s:%d\n", ip_addr, client_port);

        // send welcome message to new connection
        bytes_sent = send(client_sfd, intro_msg, strlen(intro_msg) + 1, 0);
        if (bytes_sent == -1) {
          fprintf(stderr, "send() error when sending intro msg: %s\n",
                  strerror(errno));
        }

        while(true) {
            // 5. check if we need to recv anything first
            bytes_received = recv(client_sfd, buffer, sizeof(buffer), 0);
            if (bytes_received == -1) {
                fprintf(stderr, "recv() error during transcieving: %s\n", strerror(errno));
            } else if (bytes_received == 0) {
                printf("[SERVER] Remote side has closed the connection\n");
                close(client_sfd);
                break;
            } else {
                printf("[CLIENT] %s", buffer);
            }

            // 6. send the echo back
            bytes_sent = send(client_sfd, buffer, strlen(buffer) + 1, 0);
            if (bytes_sent == -1) {
                fprintf(stderr, "send() error during transcieving: %s\n", strerror(errno));
            }

            // 7. clear buffer, get ready to recv() next msg
            memset(buffer, 0, sizeof(buffer));
        }
    }

    close(server_sfd);
    freeaddrinfo(servinfo);
    return 0;
}
