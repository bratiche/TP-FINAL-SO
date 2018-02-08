#ifndef TPE_FINAL_SO_SERVER_H
#define TPE_FINAL_SO_SERVER_H

#define BUFFER_SIZE  4098
#define DEFAULT_PORT 12345

typedef struct server * Server;

/** Data associated with a client */
typedef struct {
    int client_fd;
    char buffer[BUFFER_SIZE];
} ClientData;

/** Setup and initialization of a TCP server in the specified port */
Server server_init(int port);

/** Waits for incoming connections and returns a pointer to a new client structure */
ClientData * server_accept_connection(Server server);

ssize_t server_read_request(Server server, ClientData * data);

ssize_t server_send_response(Server server, ClientData * data);

/** Closes connection with client */
void server_close_connection(Server server, ClientData * data);

/** Closes the server and frees resources */
void server_close(Server server);

#endif //TPE_FINAL_SO_SERVER_H
