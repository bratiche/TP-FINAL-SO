#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <syslog.h>
#include <getopt.h>
#include <ctype.h>
#include "server.h"
#include "../utils.h"

/**
 * Multi-threaded concurrent server implementation
 * ...
 * ...
 */

static Server server;

/** Creates a new thread to handle a client connection */
static void new_thread(ClientData * data);

/** Single connection handler */
static void * handle_connection(void* data);

void parse_options(int argc, char **argv, int * port, char ** filename) {
    opterr = 0;
    /* p: option e requires argument p:: optional argument */
    int c;
    while ((c = getopt (argc, argv, "p:f:")) != -1) {
        switch (c) {
            /* Server port number */
            case 'p':
                *port = parse_port(optarg);
                break;
            /* Database file name */
            case 'f':
                *filename = optarg;
                break;
            case '?':
                if (optopt == 'p' || optopt == 'f')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                exit(1);
            default:
                abort();
        }
    }
}

int main(int argc, char *argv[]) {

    int server_port = DEFAULT_PORT;
    char * filename = DEFAULT_DATABASE_FILENAME;

    parse_options(argc, argv, &server_port, &filename);

    server = server_init(server_port, filename);
    if (server == NULL) {
        fprintf(stderr, "Server initialization failed\n");
        return -1;
    }

    printf("Successful database setup: '%s'\n", filename);
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