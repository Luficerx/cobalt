#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

#include "array.h"
#include "arena.h"
#include "token.h"

#define TOKEN(token) token = (Token){0}

typedef struct Parser Parser;
typedef struct Token Token;

typedef struct Lexer {
    StringBuilder source;
    StringArena *sa;
    const char *file;
    size_t pos;
} Lexer;

// FORWARD DECLARATION

// Initializes default fields for `Lexer` struct.
bool lexer_init(Lexer *lexer, const char *filepath);

// Free the memory allocated
void lexer_destroy(Lexer *lexer);

/*
    Loads `filepath` contents into `lexer->source`.
    This function returns `false` if the file is empty or if any error happen.
*/
bool lexer_load_source(Lexer *lexer, const char *filepath);

/*
    Performs the entire tokenization process of the source code loaded.
    This function returns `false` if any error happens, Logging is done in place.
*/
bool lexer_tokenize(Lexer *lexer, Parser *parser);

// Append the character to `sb` and advances the cursor to the next character.
void lexer_advance(StringBuilder *sb, StringBuilder sc, size_t *i);

/*
    Checks if `c` matches any of the existing tokens in `TokenKind` enum.
    If `c` matches any of the tokens, this function also sets each of the
    fields in `token` and might advance `i` if specials cases are encountered.
*/
bool lexer_char_is(StringBuilder source, StringBuilder *sb, Token *token, char c, size_t *i, TokenMode *mode);

// Checks if `c` is a space or a new line (`\s` or `\n`)
bool lexer_char_space_or_nline(char c);

// Checks if `c` is in range `[a-z]` and `[A-Z]`.
bool lexer_char_in_az(char c);

/*
    Checks if the next character `c` is equal to `target`.
    This function returns `false` if `i` is greater than or equal to `sb.len`.
*/
bool lexer_next_char_is(StringBuilder sb, size_t i, char target);

// Checks if `c` is in range `[0-9]`.
bool lexer_char_in_09(char c);

/*
    Checks if the next character `c` is in range `[0-9]`.
    This function returns `false` if `i` is greater than or equal to `sb.len`.
*/
bool lexer_next_char_in_09(StringBuilder sb, size_t i);

/*
Checks if the `token->lexeme` is an keyword.
This function also sets `token->kind` to `TK_KEYWORD` if a keyword was found.
*/
bool lexer_lexeme_is_keyword(Token *token);

// Print the contents of `Lexer` struct.
void lexer_log(Lexer lexer);

// Print `c`.
void lexer_log_char(char c);

#endif // LEXER_H