#include <stddef.h>
#include <stdbool.h>

#include "parser.h"
#include "token.h"
#include "sb.h"

#ifndef LEXER_H
#define LEXER_H

typedef struct Lexer {
    StringBuilder source;
    const char *file;
    size_t pos;
} Lexer;

// FORWARD DECLARATION
bool lexer_load_source(Lexer *lexer, const char *filepath);
bool lexer_tokenize(Lexer *lexer, Parser *parser);

bool lexer_init(Lexer *lexer, const char *filepath);
void lexer_destroy(Lexer *lexer);

bool lexer_char_is(StringBuilder sb, Token *token, char c, size_t *i);
bool lexer_char_in_az(char c);
bool lexer_char_in_09(char c);

bool lexer_lexeme_is_keyword(Token *token);
bool lexer_next_char_is(StringBuilder sb, size_t i, char target);

bool lexer_char_space_or_nline(char c);

void lexer_log_char(char c);


#endif // LEXER_H