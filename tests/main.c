#include <stdio.h>
#include <stdbool.h>
#include "request_parser_test.h"

extern bool debug;

int main(int argc, char *argv[]) {
    debug = true;
    printf("*********Request parser test********\n");
    request_parser_test();
    printf("************************************\n");
    return 0;
}

