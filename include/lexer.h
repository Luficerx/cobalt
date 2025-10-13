#include <stddef.h>
#include <stdbool.h>

#include "parser.h"
#include "token.h"
#include "sb.h"

#ifndef LEXER_H
#define LEXER_H

typedef struct Lexer {
    const char *file;
    StringBuilder source;
    size_t pos;
} Lexer;

// FORWARD DECLARATION
bool lexer_load_source(Lexer *lexer, const char *filepath);
bool lexer_init(Lexer *lexer, const char *filepath);
bool lexer_get_token(Token *token, char c);
bool lexer_next_char_is_token(StringBuilder sb, size_t i);
bool lexer_next_char_is_delim(StringBuilder sb, size_t i);
bool lexer_tokenize(Lexer *lexer, Parser *parser);
void lexer_log(Lexer lexer);

#endif // LEXER_H