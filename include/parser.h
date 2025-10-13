#include <stddef.h>
#include "token.h"

#ifndef PARSER_H
#define PARSER_H

typedef struct Parser {
    Token *tokens;
    size_t count;
    size_t size;
    size_t pos;
} Parser;

#define PARSER_PREP_ARRAY(ps_array, min_size)                                                             \
do {                                                                                                      \
    if ((min_size) > (ps_array)->size) {                                                                  \
        if ((ps_array)->size == 0) {                                                                      \
            (ps_array)->size = sizeof(Parser);                                                            \
        }                                                                                                 \
        while ((min_size) > (ps_array)->size) {                                                           \
            (ps_array)->size *= 2;                                                                        \
        }                                                                                                 \
        (ps_array)->tokens = realloc((ps_array)->tokens, (ps_array)->size * sizeof(*(ps_array)->tokens)); \
        assert((ps_array)->tokens != NULL);                                                               \
    }                                                                                                     \
} while (0)                                                                                               \

#define PARSER_PUSH_TOKEN(ps, tk)           \
do {                                        \
    PARSER_PREP_ARRAY((ps), (ps)->count+1); \
    (ps)->tokens[(ps)->count++] = (tk);     \
} while (0)                                 \

// FORWARD DECLARATION
void parser_init(Parser *parser);
void parser_log(Parser parser);
void parser_log_tokens(Parser parser);

#endif // PARSER_H