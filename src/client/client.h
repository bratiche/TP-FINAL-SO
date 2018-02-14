#ifndef TPE_FINAL_SO_CLIENT_H
#define TPE_FINAL_SO_CLIENT_H

#define BUFFER_SIZE         4096

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 12345

typedef struct client * Client;

Client client_init(char *hostname, int port);

/** Sends message to server */
ssize_t client_send(Client client, char * buff);

/** Receives message from server */
ssize_t client_recv(Client client, char * buff);

/** Closes a client connection and frees resources */
void client_close(Client client);

#endif //TPE_FINAL_SO_CLIENT_H
