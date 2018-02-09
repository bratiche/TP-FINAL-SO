#ifndef TPE_FINAL_SO_CLIENT_H
#define TPE_FINAL_SO_CLIENT_H

typedef struct client * Client;

Client new_client(char * hostname, int port, char * client_name);

/** Sends message to server */
ssize_t client_send(Client client, char * buff);

/** Receives message from server */
ssize_t client_recv(Client client, char * buff);

/** Closes a client connection and frees resources */
void close_client(Client client);

#endif //TPE_FINAL_SO_CLIENT_H
