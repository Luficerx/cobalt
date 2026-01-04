#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "lexer.h"
#include "parser.h"
#include "core.h"

bool lexer_init(Lexer *lexer, const char *filepath) {
    lexer->source = (StringBuilder){0};
    lexer->sa = sa_init();
    lexer->file = filepath;
    lexer->pos = 0;
    
    if (!lexer_load_source(lexer, filepath)) return false;
    
    return true;
}

void lexer_destroy(Lexer *lexer) {
    array_free(&lexer->source);
    sa_destroy(lexer->sa);

    lexer->sa = NULL;
    lexer->file = NULL;
    lexer->pos = 0;
}

bool lexer_load_source(Lexer *lexer, const char *filepath) {
    FILE *fptr;

    if ((fptr = fopen(filepath, "r")) == NULL) {
        fprintf(stderr, CORE_RED"fatal error: "CORE_END"could not read file %s\n", filepath);
        return false;
    }
    
    char c;

    for (c = fgetc(fptr); c != EOF; c = fgetc(fptr)) {
        sb_append(&lexer->source, c);
    }

    // Push EOF into the string;
    sb_append(&lexer->source, c);

    // Assumes that we only found EOF.
    if (lexer->source.len == 1) {
        fprintf(stderr, CORE_RED"fatal error: "CORE_END"file is empty: %s\n", filepath);
        return false;
    }

    fclose(fptr);
    return true;
}

bool lexer_tokenize(Lexer *lexer, Parser *parser) {
    StringBuilder line = {0};
    StringBuilder sb = {0};
    Token token = {0};
    TokenMode mode = 0;

    size_t column = 1;
    size_t pos = 0;
    
    for (size_t i = 0; i < lexer->source.len; ++i) {
        char c = lexer->source.items[i];
        ++pos;

        /*  HACK: (25-12-05 22:30:36)
            Construct the line for reporting purposes, dumb approach!
        */
        sb_append(&line, c);
        
        switch (mode) {
            case TM_NONE: break;
            case TM_STRING_LIT: {
                /*  NOTE: (25-12-05 20:32:04)
                    I have a feeling the second checking here can bring problems in the future */

                if (c == '\n' || c == EOF) {
                    sb_append(&sb, '\0');
                    fprintf(stderr, "%s", line.items);
                    fprintf(stderr, "%*s^\n", (int)pos-1, "~~~");
                    fprintf(stderr, "%s:%ld:%ld"CORE_RED" fatal error: "CORE_END"Unterminated string literal\n", lexer->file, column, pos);
                    return false;
                }

                if (c == '\\') {
                    sb_append(&sb, '\\');
                    lexer_advance(&sb, lexer->source, &i);
                    continue;
                }

                if ((sb.items[0] == '"' && c == '"') || (sb.items[0] == '\'' && c == '\'')) {
                    sb_append(&sb, c);
                    sb_append(&sb, '\0');
                    token.lexeme = sa_dup(lexer->sa, sb.items);
                    token.kind = TK_STRING_LIT;
                    
                    array_append(parser, token);
                    sb_clear(&sb);
                    
                    token = (Token){0};
                    mode = TM_NONE;
                    continue;
                }
                sb_append(&sb, c);
                continue;
            }

            case TM_NUMBER_LIT: {
                if (!isdigit(c) && c != '_' && sb.items[sb.len] != 'f' && c != 'f') {
                    sb_append(&sb, '\0');
                    token.lexeme = sa_dup(lexer->sa, sb.items);
                    token.kind = TK_NUMBER_LIT;
                    
                    array_append(parser, token);
                    sb_clear(&sb);
                    
                    token = (Token){0};
                    mode = TM_NONE;
                    break;
                } else {
                    sb_append(&sb, c);
                    continue;
                }
            }

            case TM_HEX_LIT: {
                if (!lexer_char_in_az(c) && !lexer_char_in_09(c)) {
                    sb_append(&sb, '\0');
                    token.lexeme = sa_dup(lexer->sa, sb.items);
                    token.kind = TK_HEX_LIT;
                    
                    array_append(parser, token);
                    sb_clear(&sb);
                    
                    token = (Token){0};
                    mode = TM_NONE;
                    break;
                } else {
                    sb_append(&sb, c);
                    continue;        
                }
            }

            case TM_MCOMMENT: {
                if (c == '*' && lexer_next_char_is(lexer->source, i, '/')) {
                    mode = TM_NONE;
                    ++i;
                    continue;
                }
                        
                if (c == '\n') {
                    column += 1;
                    pos = 0;
                }
                continue;
            };

            case TM_COMMENT: {
                if ((lexer_next_char_is(lexer->source, i, EOF))) {
                    sb_append(&sb, c);
                    mode = TM_NONE;
                }

                if (c == '\n') { mode = TM_NONE; }

                continue;
            }

            case TM_GENERIC: {
                if (!lexer_char_in_az(c) && !lexer_char_in_09(c) && (c != '_')) {
                    sb_append(&sb, '\0');
                    token.lexeme = sa_dup(lexer->sa, sb.items);
                    if (!lexer_lexeme_is_keyword(&token)) {
                        token.kind = TK_IDENTIFIER;
                    }
                    
                    array_append(parser, token);
                    sb_clear(&sb);
                    
                    token = (Token){0};
                    mode = TM_NONE;
                    break;
                } else {
                    sb_append(&sb, c);
                    continue;
                }
            }
        }
        
        /*  NOTE: (25-11-03 15:29:08)
            At some degree it's safe to assume
        */
        
        token.column = column;
        token.line = pos;

        if (c == EOF) {
            token.lexeme = "EOF";
            token.kind = TK_EOF;

            array_append(parser, token);
            token = (Token){0};
            continue;
        }

        if (isdigit(c)) {
            sb_append(&sb, c);

            if (c == '0' && lexer_next_char_is(lexer->source, i, 'x')) mode = TM_HEX_LIT;
            else mode = TM_NUMBER_LIT;
            continue;
        }

        if (c == '"' || c == '\'') {
            sb_append(&sb, c);
            mode = TM_STRING_LIT;
            continue;
        }

        if (c == '\n') {
            column += 1;
            pos = 0;
            sb_clear(&line);
        }

        if (c == '/' && lexer_next_char_is(lexer->source, i, '/')) {
            mode = TM_COMMENT;
            continue;
        }

        if (c == '/' && lexer_next_char_is(lexer->source, i, '*')) {
            mode = TM_MCOMMENT;
            continue;
        }

        if (lexer_char_in_az(c) || c == '_') {
            sb_append(&sb, c);
            mode = TM_GENERIC;
            continue;
        }

        if (lexer_char_is(lexer->source, &sb, &token, c, &i, &mode)) {
            array_append(parser, token);
            token = (Token){0};
            continue;
        }

        token.column = 0;
        token.line = 0;
    }

    sb_clear(&sb);
    return true;
}

void lexer_advance(StringBuilder *sb, StringBuilder sc, size_t *i) {
    sb_append(sb, sc.items[++(*i)]);
}

bool lexer_char_is(StringBuilder source, StringBuilder *sb, Token *token, char c, size_t *i, TokenMode *mode) {
    switch (c) {
        case '{': {
            token->lexeme = "{";
            token->kind = TK_LCBRACE;
            return true;
        }
        case '}': {
            token->lexeme = "}";
            token->kind = TK_RCBRACE;
            return true;
        }
        case '(': {
            token->lexeme = "(";
            token->kind = TK_LPAREN;
            return true;
        }
        case ')': {
            token->lexeme = ")";
            token->kind = TK_RPAREN;
            return true;
        }
        case '[': {
            token->lexeme = "[";
            token->kind = TK_LSBRACE;
            return true;
        }
        case ']': {
            token->lexeme = "]";
            token->kind = TK_RSBRACE;
            return true;
        }
        case ':': {
            token->lexeme = ":";
            token->kind = TK_COLON;
            return true;
        }
        case ';': {
            token->lexeme = ";";
            token->kind = TK_SEMICOLON;
            return true;
        }
        case ',': {
            token->lexeme = ",";
            token->kind = TK_COMMA;
            return true;
        }
        case '=': {
            if (lexer_next_char_is(source, *i, '=')) {
                token->lexeme = "==";
                token->kind = TK_EQUALS;
                ++(*i);
            } else {
                token->lexeme = "=";
                token->kind = TK_ASSIGN_OP;
            }
            return true;
        }
        case '!': {
            if (lexer_next_char_is(source, *i, '=')) {
                token->lexeme = "!=";
                token->kind = TK_NEQUALS;
                ++(*i);
            } else {
                token->lexeme = "!";
                token->kind = TK_NOT;
            }
            return true;
        }
        case '?': {
            token->lexeme = "?";
            token->kind = TK_QMARK;
            return true;
        }
        case '>': {
            if (lexer_next_char_is(source, *i, '=')) {
                token->lexeme = ">=";
                token->kind = TK_GEQUALS;
                ++(*i);
            } else if (lexer_next_char_is(source, *i, '>')) {
                token->lexeme = ">>";
                token->kind = TK_RSHIFT;
                ++(*i);
            } else {
                token->lexeme = ">";
                token->kind = TK_GREATER;
            }
            return true;
        }
        case '<': {
            if (lexer_next_char_is(source, *i, '=')) {
                token->lexeme = "<=";
                token->kind = TK_LEQUALS;
                ++(*i);
            } else if (lexer_next_char_is(source, *i, '<')) {
                token->lexeme = "<<";
                token->kind = TK_LSHIFT;
                ++(*i);
            } else {
                token->lexeme = "<";
                token->kind = TK_LESSER;
            }
            return true;
        }
        case '+': {
            if (lexer_next_char_is(source, *i, '+')) {
                token->lexeme = "++";
                token->kind = TK_INC_OP;
                ++(*i);
            } else {
                token->lexeme = "+";
                token->kind = TK_ADD;
            }
            return true;
        }
        case '-': {
            if (lexer_next_char_is(source, *i, '-')) {
                token->lexeme = "--";
                token->kind = TK_DEC_OP;
                ++(*i);

            } else if (lexer_next_char_in_09(source, *i)) {
                sb_append(sb, c);
                *mode = TM_NUMBER_LIT;
                return false;

            } else {
                token->lexeme = "-";
                token->kind = TK_SUB;
            }
            return true;
        }
        case '*': {
            if (lexer_next_char_is(source, *i, '=')) {
                token->lexeme = "*=";
                token->kind = TK_MULT_OP;
                ++(*i);
            } else {
                token->lexeme = "*";
                token->kind = TK_STAR;
            }
            return true;
        }
        case '^': {
            token->lexeme = "^";
            token->kind = TK_XOR;
            return true;
        }
        case '%': {
            token->lexeme = "%%";
            token->kind = TK_MOD;
            return true;
        }
        case '.': {
            token->lexeme = ".";
            token->kind = TK_DOT;
            return true;
        }
        case '/': {
            if (lexer_next_char_is(source, *i, '=')) {
                token->lexeme = "/=";
                token->kind = TK_DIV_OP;
                ++(*i);
            } else {
                token->lexeme = "/";
                token->kind = TK_SLASH;
            }
            return true;
        }
        case '|': {
            if (lexer_next_char_is(source, *i, '|')) {
                token->lexeme = "||";
                token->kind = TK_OR_OP;
                ++(*i);
            } else {
                token->lexeme = "|";
                token->kind = TK_PIPE;
            }
            return true;
        }
        case '@': {
            token->lexeme = "@";
            token->kind = TK_AT;
            return true;
        }
        case '#': {
            token->lexeme = "#";
            token->kind = TK_HASH;
            return true;
        }
        case '&': {
            if (lexer_next_char_is(source, *i, '&')) {
                token->lexeme = "&&";
                token->kind = TK_AND_OP;
                ++i;
            } else {
                token->lexeme = "&";
                token->kind = TK_AMPER;
            }
            return true;
        }
        case '\\': {
            token->lexeme = "\\";
            token->kind = TK_STRAY;
            return true;
        }
    }
    return false;
}

bool lexer_char_space_or_nline(char c) {
    return (c == ' ' || c == '\n');
}

bool lexer_char_in_az(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool lexer_next_char_is(StringBuilder sb, size_t i, char target) {
    if (i >= sb.len) return false;
    return (sb.items[i+1] == target);
}

bool lexer_char_in_09(char c) {
    return (c >= '0' && c <= '9');
}

bool lexer_next_char_in_09(StringBuilder sb, size_t i) {
    if (i >= sb.len) return false;
    char c = sb.items[i+1];
    return lexer_char_in_09(c);
}

bool lexer_lexeme_is_keyword(Token *token) {
    if (strcmp("import", token->lexeme) == 0 ||
        strcmp("static", token->lexeme) == 0 ||
        strcmp("sizeof", token->lexeme) == 0 ||
        strcmp("return", token->lexeme) == 0 ||
        strcmp("macro", token->lexeme) == 0 ||
        strcmp("const", token->lexeme) == 0 ||

        strcmp("if", token->lexeme) == 0 ||
        strcmp("elif", token->lexeme) == 0 ||
        strcmp("else", token->lexeme) == 0 ||
        strcmp("pass", token->lexeme) == 0) {
        token->kind = TK_KEYWORD;
        return true;
    }
    
    return false;
}

void lexer_log(Lexer lexer) {
    printf("Lexer Log:\n");
    printf("  file - %s\n", lexer.file);
    printf("  pos - %ld\n", lexer.pos);
    printf("Lexer Source:\n");
    printf("  length - %ld\n", lexer.source.len);
    printf("  bytes - %ld\n", lexer.source.size);
}

void lexer_log_char(char c) {
    printf("%c\n", c);
}