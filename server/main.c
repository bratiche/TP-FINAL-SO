#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <syslog.h>
#include <limits.h>
#include <errno.h>
#include "server.h"

/**
 * Multi-threaded server implementation
 * ...
 * ...
 */

static Server server;

/** Creates a new thread to handle a client connection */
static void new_thread(ClientData * data);

/** Single connection handler */
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

    server = server_init(server_port);
    if (server == NULL) {
        fprintf(stderr, "Server initialization failed\n");
        return -1;
    }

    printf("Listening on TCP port %d\n", server_port);
    printf("Waiting for connections...\n");

    while(true) {
        ClientData * data = server_accept_connection(server);

        if (data != NULL) {
            new_thread(data);
            syslog(LOG_DEBUG, "[SERVER] [+] socket %d", data->client_fd);
        } else {
            fprintf(stderr, "Connection error\n");
        }
    }

//unreachable code

//    server_close(server);
//
//    return 0;
}

void new_thread(ClientData * data) {
    pthread_t thread;

    if (pthread_create(&thread, NULL, handle_connection, data) == 0) {
        pthread_detach(thread);
    }
}

void * handle_connection(void* data) {
    ssize_t n;

    do {
        n = server_read_request(server, (ClientData *)data);

        if (n > 0) {
            server_send_response(server, (ClientData *)data);
        }
    } while (n > 0);

    syslog(LOG_DEBUG, "[SERVER] [-] socket %d", ((ClientData *)data)->client_fd);
    server_close_connection(server, data);
    
    return 0;
}