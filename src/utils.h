#ifndef TPE_FINAL_SO_UTILS_H
#define TPE_FINAL_SO_UTILS_H

#include "parser.h"

int parse_port(char *optarg);

void print_state(const char *p, const char *(*namefnc)(unsigned), const ParserEvent *e);

#endif //TPE_FINAL_SO_UTILS_H
