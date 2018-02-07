#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <memory.h>
#include <syslog.h>
#include <limits.h>
#include <errno.h>
#include "server.h"

typedef struct {
    int client_fd;
} ThreadData;

static void new_thread(int client_fd);
static void * handle_connection(void* data);

int parse_port(char *optarg) {
    char *end = 0;
    long sl   = strtol(optarg, &end, 10);

    if (end == optarg|| '\0' != *end
        || ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno)
        || sl < 0 || sl > USHRT_MAX) {
        fprintf(stderr, "port should be an integer: %s\n", optarg);
        exit(1);
    }

    return (int) sl;
}

int main(int argc, char *argv[]) {

    int server_port = DEFAULT_PORT;
    if (argc >= 2) {
        server_port = parse_port(argv[1]);
    }

    Server server = server_init(server_port);
    if (server == NULL) {
        fprintf(stderr, "Server initialization failed\n");
        return -1;
    }

    printf("Listening on TCP port %d\n", server_port);
    printf("Waiting for connections...\n");

    while(true) {
        int client_fd = accept_connection(server);

        if (client_fd >= 0) {
            new_thread(client_fd);
            syslog(LOG_DEBUG, "[SERVER] [+] socket %d", client_fd);
        } else {
            fprintf(stderr, "Connection error\n");
        }
    }

//unreachable code

//    server_close(server);
//
//    return 0;
}

/** Creates a new thread to handle a client connection */
void new_thread(int client_fd) {
    pthread_t thread;

    ThreadData * data = malloc(sizeof(ThreadData));
    data->client_fd = client_fd;

    if (pthread_create(&thread, NULL, handle_connection, data) == 0) {
        pthread_detach(thread);
    }
}

/** */
void * handle_connection(void* data) {
    int client_fd = ((ThreadData *)data)->client_fd;

    char buffer[BUFFER_SIZE];
    ssize_t read;

    do {
        bzero(buffer, BUFFER_SIZE);
        read = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (read > 0) {
            printf("%s", buffer);
            send(client_fd, buffer, strlen(buffer), MSG_NOSIGNAL);
        }
    } while (read > 0);

    syslog(LOG_DEBUG, "[SERVER] [-] socket %d", client_fd);
    close(client_fd);
    free(data);
    return 0;
}