/**
 * Backend
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <memory.h>
#include <sys/syslog.h>
#include <unistd.h>
#include "client.h"

/** Tamanio de los buffers de I/O */
#define BUFFER_SIZE 4096

/** Estructura cliente */
struct client {
    int fd;
    char * name;

    // informacion del server
    struct sockaddr_storage server_address;
    socklen_t               server_address_len;
    int                     server_domain;
};

static int resolve_server_address(char * hostname, int port, Client client);
static int connect_to_server(Client client);

Client new_client(char * hostname, int port, char * client_name) {

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

    client->name = client_name;

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
    return send(client->fd, buff, strlen(buff), MSG_NOSIGNAL);
}

ssize_t client_recv(Client client, char * buff) {
    return recv(client->fd, buff, BUFFER_SIZE, 0);
}

void close_client(Client client) {
    close(client->fd);
    free(client);
}
