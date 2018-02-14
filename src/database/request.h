#ifndef TPE_FINAL_SO_REQUEST_H
#define TPE_FINAL_SO_REQUEST_H

#include "../protocol.h"

#define BUFFER_SIZE 4096

typedef struct {
    int type;
    int argc;
    char * args[MAX_ARGS];
} Request;


Request * new_request(void);

void process_request(int state, Request * request);

void print_request(Request * request);

void destroy_request(Request * request);

#endif //TPE_FINAL_SO_REQUEST_H
