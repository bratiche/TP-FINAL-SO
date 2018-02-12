#ifndef TPE_FINAL_SO_REQUEST_H
#define TPE_FINAL_SO_REQUEST_H

#define MAX_ARGS    5
#define ARG_SIZE    50

typedef struct {
    int type;
    int argc;
    char * args[MAX_ARGS];
} Request;

Request * new_request(void);

void process_request(Request * request, char * buffer);

void print_request(Request * request);

void destroy_request(Request * request);

#endif //TPE_FINAL_SO_REQUEST_H
