#include <stdlib.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <netinet/in.h>
#include <memory.h>
#include <unistd.h>
#include "server.h"

#define PENDING_CONNECTIONS 10
#define DATABASE_PROC       "database"

struct server {
    int listen_socket;
    // Write in db_in, read from db_out
    int database_in, database_out;

    struct sockaddr_in address;
    socklen_t          address_len;
};

/** Forks database handler process and creates pipes for inter-process communication */
static int database_init(Server server);

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

    if (database_init(server) < 0) {
        free(server);
        return NULL;
    }

    return server;
}

ClientData * server_accept_connection(Server server) {
    int client_socket = accept(server->listen_socket, 0, 0);

    if (client_socket < 0) {
        perror("accept() failed");
        return NULL;
    }

    ClientData * ret = malloc(sizeof(*ret));
    if (ret != NULL) {
        ret->client_fd = client_socket;
    }
    
    return ret;
}

int database_init(Server server) {

    //create pipes, bytes written on db_...[1] can be read from db_...[0]
    int db_in[2];
    int db_out[2];

    int r1 = pipe(db_in);
    int r2 = pipe(db_out);

    if (r1 < 0 || r2 < 0) {
        perror("pipe() failed");
        return -1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error starting database\n");
        return -1;
    } else if (pid == 0) {
        dup2(db_in[0], STDIN_FILENO);
        dup2(db_out[1], STDOUT_FILENO);

        close(db_in[1]);
        close(db_out[0]);

        int value = execve(DATABASE_PROC, NULL, NULL);
        if (value < 0) {
            perror("execve() failed");
            return -1;
        }
    } else {
        close(db_in[0]);
        close(db_out[1]);

        server->database_in = db_in[1];
        server->database_out = db_out[0];
    }

    return 0;
}

ssize_t server_read_request(Server server, ClientData * data) {
    int client_fd = data->client_fd;
    char * buffer = data->buffer;
    ssize_t n;

    bzero(buffer, BUFFER_SIZE);
    n = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (n > 0) {
        n = write(server->database_in, buffer, strlen(buffer));
    }

    printf("%s", buffer);

    return n;
}

ssize_t server_send_response(Server server, ClientData * data) {
    int client_fd = data->client_fd;
    char * buffer = data->buffer;
    ssize_t n;

    bzero(buffer, BUFFER_SIZE);
    n = read(server->database_out, buffer, BUFFER_SIZE);
    if (n > 0) {
        n = send(client_fd, buffer, strlen(buffer), MSG_NOSIGNAL);
    }

    return n;
}

void server_close_connection(Server server, ClientData * data) {
    close(data->client_fd);
    free(data);
}

void server_close(Server server) {
    close(server->listen_socket);
    free(server);
}