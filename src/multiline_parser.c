#include "multiline_parser.h"

const char * multiline_event(enum multiline_event_type type) {
    const char *ret = NULL;
    switch(type) {
        case MULTI_BYTE:
            ret = "byte(c)";
            break;
        case MULTI_NEWLINE:
            ret = "newline(c)";
            break;
        case MULTI_WAIT:
            ret = "wait()";
            break;
        case MULTI_FIN:
            ret = "fin()";
            break;
    }
    return ret;
}

enum {
    BYTE,
    NEWLINE,
    DOT,
    FIN
};

///////////////////////////////////////////////////////////////////////////////
// Declaración formal

static void byte(ParserEvent *ret, const uint8_t c) {
    ret->type    = MULTI_BYTE;
    ret->n       = 1;
    ret->data[0] = c;
}

static void wait(ParserEvent *ret, const uint8_t c) {
    ret->type    = MULTI_WAIT;
    ret->n       = 0;
}

static void newline(ParserEvent *ret, const uint8_t c) {
    ret->type    = MULTI_NEWLINE;
    ret->n       = 1;
    ret->data[0] = c;
}

static void dot(ParserEvent *ret, const uint8_t c) {
    newline(ret, '.');
}

static void fin(struct parser_event *ret, const uint8_t c) {
    ret->type    = MULTI_FIN;
    ret->n       = 0;
}

static const ParserStateTransition ST_BYTE[] = {
    {.when = '\n',      .dest = NEWLINE,        .act1 = wait,},
    {.when = ANY,       .dest = BYTE,           .act1 = byte,},
};

static const ParserStateTransition ST_NEWLINE[] = {
    {.when = '.',       .dest = DOT,            .act1 = wait,},
    {.when = '\n',      .dest = NEWLINE,        .act1 = wait,},
    {.when = ANY,       .dest = BYTE,           .act1 = newline,},
};

static const ParserStateTransition ST_DOT[] = {
    {.when = '\n',      .dest = FIN,            .act1 = fin,},
    {.when = ANY,       .dest = BYTE,           .act1 = dot, .act2 = byte},
};

static const ParserStateTransition ST_FIN[] = {
    {.when = ANY,      .dest = FIN,             .act1 = fin,},
};

static const ParserStateTransition *states[] = {
        ST_BYTE,
        ST_NEWLINE,
        ST_DOT,
        ST_FIN,
};

#define N(x) (sizeof(x)/sizeof((x)[0]))

static const size_t states_n [] = {
        N(ST_BYTE),
        N(ST_NEWLINE),
        N(ST_DOT),
        N(ST_FIN),
};

static ParserDefinition definition = {
        .states_count = N(states),
        .states       = states,
        .states_n     = states_n,
        .start_state  = BYTE,
};

const ParserDefinition * multiline_parser_definition(void) {
    return &definition;
}