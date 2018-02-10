#include "request_parser.h"

const char * request_event(enum request_event_type type) {
    const char *ret = NULL;
    switch(type) {
        case REQUEST_CMD:
            ret = "cmd(c)";
            break;
        case REQUEST_NEWARG:
            ret = "newarg(c)";
            break;
        case REQUEST_ARG:
            ret = "arg(c)";
            break;
        case REQUEST_WAIT:
            ret = "wait()";
            break;
        case REQUEST_FIN:
            ret = "fin()";
            break;
    }
    return ret;
}

enum {
    CMD,
    NEWLINE,
    ARG,
    DOT,
    FIN
};

///////////////////////////////////////////////////////////////////////////////
// Declaración formal

static void cmd(ParserEvent *ret, const uint8_t c) {
    ret->type    = REQUEST_CMD;
    ret->n       = 1;
    ret->data[0] = c;
}

static void wait(ParserEvent *ret, const uint8_t c) {
    ret->type    = REQUEST_WAIT;
    ret->n       = 0;
}

static void new_arg(ParserEvent *ret, const uint8_t c) {
    ret->type    = REQUEST_NEWARG;
    ret->n       = 1;
    ret->data[0] = c;
}

static void arg(ParserEvent *ret, const uint8_t c) {
    ret->type    = REQUEST_ARG;
    ret->n       = 1;
    ret->data[0] = c;
}

static void dot(ParserEvent *ret, const uint8_t c) {
    new_arg(ret, '.');
}

static void fin(struct parser_event *ret, const uint8_t c) {
    ret->type    = REQUEST_FIN;
    ret->n       = 0;
}

static const ParserStateTransition ST_CMD[] = {
    {.when = '\n',      .dest = NEWLINE,        .act1 = wait,},
    {.when = ANY,       .dest = CMD,            .act1 = cmd  },
};

static const ParserStateTransition ST_NEWLINE[] = {
    {.when = '.',       .dest = DOT,            .act1 = wait,},
    {.when = ANY,       .dest = ARG,            .act1 = new_arg,},
};

static const ParserStateTransition ST_ARG[] = {
    {.when = '\n',      .dest = NEWLINE,        .act1 = wait,},
    {.when = ANY,       .dest = ARG,            .act1 = arg,},
};

static const ParserStateTransition ST_DOT[] = {
    {.when = '\n',      .dest = FIN,            .act1 = fin,},
    {.when = ANY,       .dest = ARG,            .act1 = dot, .act2 = arg},
};

static const ParserStateTransition ST_FIN[] = {
    {.when = ANY,      .dest = FIN,             .act1 = fin,},
};

static const ParserStateTransition *states[] = {
        ST_CMD,
        ST_NEWLINE,
        ST_ARG,
        ST_DOT,
        ST_FIN,
};

#define N(x) (sizeof(x)/sizeof((x)[0]))

static const size_t states_n [] = {
        N(ST_CMD),
        N(ST_NEWLINE),
        N(ST_ARG),
        N(ST_DOT),
        N(ST_FIN),
};

static ParserDefinition definition = {
        .states_count = N(states),
        .states       = states,
        .states_n     = states_n,
        .start_state  = CMD,
};

const ParserDefinition * request_parser_definition(void) {
    return &definition;
}
