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
#include <signal.h>
#include "server.h"

typedef struct {
    int client_fd;
} ThreadData;

/** Creates a new thread to handle a client connection */
static void new_thread(int client_fd);

/** */
static void * handle_connection(void* data);

/** Forks database handler process and creates pipes for inter-process communication */
static void database_init(void);

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
    
    //signal(SIGPIPE, SIG_IGN);

    database_init();

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

#define DATABASE_PROC "database"

// Write in db_in, read from db_out
int database_in, database_out;

void database_init(void) {

    //create pipes, bytes written on db_...[1] can be read from db_...[0]
    int db_in[2];
    int db_out[2];

    int r1 = pipe(db_in);
    int r2 = pipe(db_out);

    if (r1 < 0 || r2 < 0) {
        perror("pipe() failed");
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error starting database\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        dup2(db_in[0], STDIN_FILENO);
        dup2(db_out[1], STDOUT_FILENO);

        close(db_in[1]);
        close(db_out[0]);

        int value = execve(DATABASE_PROC, NULL, NULL);
        if (value < 0) {
            perror("execve() failed");
        }
    } else {
        close(db_in[0]);
        close(db_out[1]);
        
        database_in = db_in[1];
        database_out = db_out[0];
    }
}


void new_thread(int client_fd) {
    pthread_t thread;

    ThreadData * data = malloc(sizeof(ThreadData));
    data->client_fd = client_fd;

    if (pthread_create(&thread, NULL, handle_connection, data) == 0) {
        pthread_detach(thread);
    }
}

void * handle_connection(void* data) {
    int client_fd = ((ThreadData *)data)->client_fd;

    char buffer[BUFFER_SIZE];
    ssize_t n;

    do {
        bzero(buffer, BUFFER_SIZE);
        n = recv(client_fd, buffer, BUFFER_SIZE, 0);
        write(database_in, buffer, strlen(buffer));

        if (n > 0) {
            printf("%s", buffer);
            bzero(buffer, BUFFER_SIZE);
            read(database_out, buffer, BUFFER_SIZE);
            send(client_fd, buffer, strlen(buffer), MSG_NOSIGNAL);
        }
    } while (n > 0);

    syslog(LOG_DEBUG, "[SERVER] [-] socket %d", client_fd);
    close(client_fd);
    free(data);
    return 0;
}