#ifndef TPE_FINAL_SO_SERVER_H
#define TPE_FINAL_SO_SERVER_H

#define BUFFER_SIZE  4098
#define DEFAULT_PORT 12345

typedef struct server * Server;

/** Setup and initialization of a TCP server in the specified port */
Server server_init(int port);

/** Closes the server and frees resources */
void server_close(Server server);

/** Waits for incoming connections and returns a file descriptor for the new client socket */
int accept_connection(Server server);

#endif //TPE_FINAL_SO_SERVER_H
