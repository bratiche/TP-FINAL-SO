#include <stdlib.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <netinet/in.h>
#include <memory.h>
#include <unistd.h>
#include "server.h"

#define PENDING_CONNECTIONS 10

struct server {
    int listen_socket;

    struct sockaddr_in address;
    socklen_t          address_len;
};

int create_master_socket(int protocol, struct sockaddr *addr, socklen_t addr_len) {
    int sock_opt = true;

    int sock = socket(addr->sa_family, SOCK_STREAM, protocol);

    if (sock < 0) {
        perror("socket() failed");
        return -1;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&sock_opt, sizeof(sock_opt)) < 0) {
        perror("setsockopt() failed");
        return -1;
    }
    
    if(bind(sock, addr, addr_len) < 0) {
        perror("bind() failed");
        return -1;
    }

    if(listen(sock, PENDING_CONNECTIONS) != 0) {
        perror("listen() failed");
        return -1;
    }

    return sock;
}

Server server_init(int port) {
    Server server = malloc(sizeof(struct server));

    if (server == NULL) {
        return NULL;
    }

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr);

    addr.sin_port = htons((uint16_t) port);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;

    memcpy(&server->address, &addr, addr_len);
    server->address_len = addr_len;

    server->listen_socket = create_master_socket(IPPROTO_TCP, (struct sockaddr *)&server->address, server->address_len);

    if (server->listen_socket < 0) {
        free(server);
        return NULL;
    }

    return server;
}

int accept_connection(Server server) {

    int client_socket = accept(server->listen_socket, 0, 0);

    if (client_socket < 0) {
        perror("accept() failed");
        return -1;
    }

    return client_socket;
}

void server_close(Server server) {
    close(server->listen_socket);
    free(server);
}

//Request read_request(int client_fd) {
//
//}
//
//void process_request(Request request, int client_fd) {
//
//}
//
//Response send_response(Request request, int client_fd) {
//
//}