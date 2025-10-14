#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "lexer.h"
#include "parser.h"
#include "core.h"

bool lexer_load_source(Lexer *lexer, const char *filepath) {
    FILE *fptr;

    if ((fptr = fopen(filepath, "r")) == NULL) {
        fprintf(stderr, "\033[0;31mfatal error:\033[0m could not read file %s\n", filepath);
        return false;
    }

    for (char c = fgetc(fptr); c != EOF; c = fgetc(fptr)) {
        SB_PUSH_CHAR(&lexer->source, c);
    }

    fclose(fptr);
    return true;
}

bool lexer_init(Lexer *lexer, const char *filepath) {
    printf("Tokenizing file: "CORE_RED"%s"CORE_END"\n", filepath);
    
    lexer->source = (StringBuilder){0};
    lexer->file = filepath;
    lexer->pos = 0;
    
    if (!lexer_load_source(lexer, filepath)) { 
        fprintf(stderr, "\033[0;31mlexer error:\033[0m could not parse file: %s", filepath);
        return false; 
    }

    return true;
}

void lexer_destroy(Lexer *lexer) {
    free(lexer->source.string);
}

bool lexer_tokenize(Lexer *lexer, Parser *parser) {
    StringBuilder sb = {0};
    Token token = {0};
    TokenMode mode;

    size_t column = 0;
    
    for (size_t i = 0; i <= lexer->source.length; ++i) {
        char c = lexer->source.string[i];

        switch (mode) {
            case TM_NONE: break;

            case TM_STRING_LIT: { 
                if (c == '"') {
                    SB_PUSH_CHAR(&sb, c);
                    SB_PUSH_CHAR(&sb, '\0');
                    token.lexeme = strdup(sb.string);
                    token.kind = TK_STRING_LIT;
                    
                    PARSER_PUSH_TOKEN(parser, token);
                    SB_CLEAR(&sb);
                    
                    token = (Token){0};
                    mode = TM_NONE;
                    continue;
                }
                SB_PUSH_CHAR(&sb, c);
                continue;
            }

            case TM_NUMBER_LIT: {
                if (!isdigit(c) && c != '_') {
                    SB_PUSH_CHAR(&sb, '\0');
                    token.lexeme = strdup(sb.string);
                    token.kind = TK_NUMBER_LIT;
                    
                    PARSER_PUSH_TOKEN(parser, token);
                    SB_CLEAR(&sb);
                    
                    token = (Token){0};
                    mode = TM_NONE;
                    break;
                } else {
                    SB_PUSH_CHAR(&sb, c);
                    continue;
                }
            }

            case TM_HEX_LIT: {
                if (!lexer_char_in_az(c) && !lexer_char_in_09(c)) {
                    SB_PUSH_CHAR(&sb, '\0');
                    token.lexeme = strdup(sb.string);
                    token.kind = TK_HEX_LIT;
                    
                    PARSER_PUSH_TOKEN(parser, token);
                    SB_CLEAR(&sb);
                    
                    token = (Token){0};
                    mode = TM_NONE;
                    break;
                } else {
                    SB_PUSH_CHAR(&sb, c);
                    continue;        
                }
            }

            case TM_MCOMMENT: {
                if (c == '*' && lexer_next_char_is(lexer->source, i, '/')) {
                    mode = TM_NONE;
                    ++i;
                    continue;
                }
                continue;
            };

            case TM_COMMENT: {
                if (c != '\n') continue;
                mode = TM_NONE;
                continue;
            }

            case TM_GENERIC: {
                if (!lexer_char_in_az(c) && !lexer_char_in_09(c) && (c != '_')) {
                    SB_PUSH_CHAR(&sb, '\0');
                    token.lexeme = strdup(sb.string);
                    if (!lexer_lexeme_is_keyword(&token)) {
                        token.kind = TK_IDENTIFIER;
                    }
                    
                    PARSER_PUSH_TOKEN(parser, token);
                    SB_CLEAR(&sb);
                    
                    token = (Token){0};
                    mode = TM_NONE;
                    break;
                } else {
                    SB_PUSH_CHAR(&sb, c);
                    continue;
                }
            }
        }

        // NOTE: At some degree it's safe to assume
        // this reads the first character of a sequence.

        if (isdigit(c)) {
            SB_PUSH_CHAR(&sb, c);
            
            if (c == '0' && lexer_next_char_is(lexer->source, i, 'x')) mode = TM_HEX_LIT;
            else mode = TM_NUMBER_LIT;
            continue;
        }
        
        if (c == '"') {
            SB_PUSH_CHAR(&sb, c);
            mode = TM_STRING_LIT;
            continue;
        }
        
        if (c == '\n')  column += 1;

        if (c == '/' && lexer_next_char_is(lexer->source, i, '/')) {
            mode = TM_COMMENT;
            continue;
        }

        if (c == '/' && lexer_next_char_is(lexer->source, i, '*')) {
            mode = TM_MCOMMENT;
            continue;
        }

        if (lexer_char_in_az(c) || c == '_') {
            SB_PUSH_CHAR(&sb, c);
            mode = TM_GENERIC;
            continue;
        }

        if (lexer_char_is(&token, c)) {
            PARSER_PUSH_TOKEN(parser, token);
            token = (Token){0};
            continue;
        }
    }

    SB_FREE(&sb);
    return true;
}

bool lexer_char_is(Token *token, char c) {
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
            token->lexeme = "=";
            token->kind = TK_ASSIGN_OP;
            return true;
        }
        case '!': {
            token->lexeme = "!";
            token->kind = TK_NOT;
            return true;
        }
        case '?': {
            token->lexeme = "?";
            token->kind = TK_QMARK;
            return true;
        }
        case '>': {
            token->lexeme = ">";
            token->kind = TK_GREATER;
            return true;
        }
        case '<': {
            token->lexeme = "<";
            token->kind = TK_LESSER;
            return true;
        }
        case '+': {
            token->lexeme = "+";
            token->kind = TK_PLUS;
            return true;
        }
        case '-': {
            token->lexeme = "-";
            token->kind = TK_MINUS;
            return true;
        }
        case '*': {
            token->lexeme = "*";
            token->kind = TK_STAR;
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
            token->lexeme = "/";
            token->kind = TK_SLASH;
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
            token->lexeme = "&";
            token->kind = TK_AMPER;
            return true;
        }
    }
    return false;
}

bool lexer_char_in_az(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool lexer_char_in_09(char c) {
    return (c >= '0' && c <= '9');
}

bool lexer_lexeme_is_keyword(Token *token) {
    if (strcmp("import", token->lexeme) == 0 ||
        strcmp("static", token->lexeme) == 0 ||
        strcmp("sizeof", token->lexeme) == 0 ||
        strcmp("return", token->lexeme) == 0 ||
        strcmp("const", token->lexeme)  == 0 ||
        strcmp("macro", token->lexeme) == 0) {
            token->kind = TK_KEYWORD;
            return true;
    }
    return false;
}

bool lexer_next_char_is(StringBuilder sb, size_t i, char target) {
    if (i >= sb.length) return false;
    return (sb.string[i+1] == target);
}

bool lexer_char_space_or_nline(char c) {
    return (c == ' ' || c == '\n');
}

void lexer_log(Lexer lexer) {
    printf("Lexer Log:\n");
    printf("  file - %s\n", lexer.file);
    printf("  pos - %ld\n", lexer.pos);
    printf("Lexer Source:\n");
    printf("  length - %ld\n", lexer.source.length);
    printf("  bytes - %ld\n", lexer.source.size);
}

void lexer_log_char(char c) {
    printf("%c\n", c);
}