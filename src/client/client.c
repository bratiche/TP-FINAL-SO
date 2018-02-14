#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <memory.h>
#include <sys/syslog.h>
#include <unistd.h>
#include "client.h"

/** Estructura cliente */
struct client {
    int fd;

    // informacion del server
    struct sockaddr_storage server_address;
    socklen_t               server_address_len;
    int                     server_domain;
};

static int resolve_server_address(char * hostname, int port, Client client);
static int connect_to_server(Client client);

Client client_init(char *hostname, int port) {

    Client client = malloc(sizeof(struct client));
    if (client == NULL) {
        return NULL;
    }

    if (resolve_server_address(hostname, port, client) < 0) {
        free(client);
        return NULL;
    }

    if (connect_to_server(client) < 0) {
        free(client);
        return NULL;
    }

    syslog(LOG_DEBUG, "[CLIENT] connected to %s:%d", hostname, port);

    return client;
}

int resolve_server_address(char * hostname, int port, Client client) {
    struct addrinfo *server_resolution = 0;

    struct addrinfo hints = {
            .ai_family    = AF_UNSPEC,    /* Allow IPv4 or IPv6 */
            .ai_socktype  = SOCK_STREAM,  /* Datagram socket */
            .ai_flags     = AI_PASSIVE,   /* For wildcard IP address */
            .ai_protocol  = 0,            /* Any protocol */
            .ai_canonname = NULL,
            .ai_addr      = NULL,
            .ai_next      = NULL,
    };

    char buff[7];
    snprintf(buff, sizeof(buff), "%hu", port);
    if (0 != getaddrinfo(hostname, buff, &hints, &server_resolution)){
        fprintf(stderr,"Domain name resolution error.\n");
        return -1;
    }

    if(server_resolution == 0) {
        fprintf(stderr, "Invalid domain.\n");
        return -1;
    }

    memcpy(&client->server_address, server_resolution->ai_addr, server_resolution->ai_addrlen);
    client->server_address_len  = server_resolution->ai_addrlen;
    client->server_domain       = server_resolution->ai_family;

    freeaddrinfo(server_resolution);

    return 0;
}

int connect_to_server(Client client) {
    // create client socket
    int fd = socket(client->server_domain, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        perror("socket() failed");
        return -1;
    }

    client->fd = fd;

    // connect to server
    if (connect(fd, (struct sockaddr *) &client->server_address, client->server_address_len) < 0) {
        perror("connect() failed");
        return -1;
    }

    return 0;
}

ssize_t client_send(Client client, char * buff) {
    ssize_t n = send(client->fd, buff, strlen(buff), MSG_NOSIGNAL);
    if (n <= 0) {
        fprintf(stderr, "Connection lost. Exiting\n");
        client_close(client);
        exit(-1);
    }
    return n;
}

ssize_t client_recv(Client client, char * buff) {
    ssize_t n = recv(client->fd, buff, BUFFER_SIZE, 0);
    if (n <= 0) {
        fprintf(stderr, "Connection lost. Exiting\n");
        client_close(client);
        exit(-1);
    }
    return n;
}

void client_close(Client client) {
    close(client->fd);
    free(client);
    syslog(LOG_DEBUG, "[CLIENT] disconnected");
}

