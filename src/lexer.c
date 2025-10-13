#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "lexer.h"
#include "parser.h"

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
    lexer->source = (StringBuilder){0};
    lexer->file = filepath;
    lexer->pos = 0;
    
    if (!lexer_load_source(lexer, filepath)) { 
        fprintf(stderr, "\033[0;31mlexer error:\033[0m could not parse file: %s", filepath);
        return false; 
    }

    return true;
}
bool lexer_get_token(Token *token, char c) {
    switch (c) {
        case '{':
            token->kind = TK_LCBRACE;
            token->lexeme = "{";
            return true;
            
        case '}':
            token->kind = TK_RCBRACE;
            token->lexeme = "}";
            return true;
            
        case '(':
            token->kind = TK_LPAREN;
            token->lexeme = "(";
            return true;
            
        case ')':
            token->kind = TK_RPAREN;
            token->lexeme = ")";
            return true;
            
        case '[':
            token->kind = TK_LSPAREN;
            token->lexeme = "[";
            return true;
            
        case ']':
            token->kind = TK_RSPAREN;
            token->lexeme = "]";
            return true;

        case ';':
            token->kind = TK_SEMICOLON;
            token->lexeme = ";";
            return true;

        case ':':
            token->kind = TK_COLON;
            token->lexeme = ":";
            return true;

        case ',':
            token->kind = TK_COMMA;
            token->lexeme = ",";
            return true;
    }
    return false;
}

bool lexer_next_char_in_zero2nine(StringBuilder sb, size_t i) {
    if (i > sb.length) return false;
    char c = sb.string[i+1];
    return (c >= '0' && c <= '9');
}

bool lexer_next_char_in_a2z(StringBuilder sb, size_t i) {
    if (i > sb.length) return false;
    char c = sb.string[i+1];
    
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool lexer_is_token(char c) { return strchr("{}()[];:,", c) != NULL; }
bool lexer_next_char_is(StringBuilder sb, size_t i, char c) { 
    if (i > sb.length) return false;
    char next_c = sb.string[i+1];
    return c == next_c;
 }

bool lexer_next_char_is_token(StringBuilder sb, size_t i) {
    if (i < sb.length) { return lexer_is_token(sb.string[i+1]); }
    return false;
}
bool lexer_next_char_is_delim(StringBuilder sb, size_t i) { 
    if (i < sb.length) { 
        if (sb.string[i+1] == '\0') return true;
        else if (sb.string[i+1] == ' ') return true;
    }
    return false;
}

bool lexer_tokenize(Lexer *lexer, Parser *parser) {
    StringBuilder sb = {0};
    Token token = {0};
    
    for (size_t i = 0; i < lexer->source.length; ++i) {
        char c = lexer->source.string[i];
    
        if (sb.length > 0) {
            switch (token.mode) {
                case TM_STRING_LIT: { 
                    SB_PUSH_CHAR(&sb, c);

                    if (c == '"') {
                        SB_PUSH_CHAR(&sb, '\0');
                        token.lexeme = strdup(sb.string);
                        token.kind = TK_IDENTIFIER;
                        
                        PARSER_PUSH_TOKEN(parser, token);
                        SB_CLEAR(&sb);
                        token = (Token){0};
                    }
                    continue;
                }
                case TM_NUMBER_LIT: {
                    SB_PUSH_CHAR(&sb, c);
                    if (!lexer_next_char_in_zero2nine(lexer->source, i)) {
                        SB_PUSH_CHAR(&sb, '\0');
                        token.lexeme = strdup(sb.string);
                        token.kind = TK_NUMBER_LIT;
                        
                        PARSER_PUSH_TOKEN(parser, token);
                        SB_CLEAR(&sb);
                        token = (Token){0};
                    }
                    continue;
                }
                case TM_GENERIC: {
                    SB_PUSH_CHAR(&sb, c);
                    
                    if (!lexer_next_char_in_a2z(lexer->source, i)) {
                        SB_PUSH_CHAR(&sb, '\0');
                        token.lexeme = strdup(sb.string);
                        token.kind = TK_IDENTIFIER;
                        
                        PARSER_PUSH_TOKEN(parser, token);
                        SB_CLEAR(&sb);
                        token = (Token){0};
                    }
                    continue;
                }
            }
        }
        
        if (c == ' ' || c == '\n') continue;
        
        if (lexer_is_token(c)) {
            lexer_get_token(&token, c);
            PARSER_PUSH_TOKEN(parser, token);
            token = (Token){0};
            continue;
        }

        SB_PUSH_CHAR(&sb, c);
        token.column = 0;
        token.line = i;

        if (isdigit(c)) { token.mode = TM_NUMBER_LIT; }
        else if (c == '"') { token.mode = TM_STRING_LIT; }
        else { token.mode = TM_GENERIC; }

        // if (lexer_get_token(&token, c)) {
        //     token.line = i;
        //     token.column = 0;
            
        //     PARSER_PUSH_TOKEN(parser, token);
        //     token = (Token){0};
        //     continue;
        // } else { token = (Token){0}; }
        
    
        // if (sb.length > 0 && (c == ' ' || c == '\n')) {
        //     SB_PUSH_CHAR(&sb, '\0');
        //     token.kind = IDENTIFIER;
        //     token.lexeme = strdup(sb.string);

        //     PARSER_PUSH_TOKEN(parser, token);
        //     SB_CLEAR(&sb);

        //     token = (Token){0};
        //     continue;
            
        // } else if (c == ' ' || c == '\n') { } else {
        //     SB_PUSH_CHAR(&sb, c);
        //     continue;
        // }

        // if (!string_lit && c == '"') {
        //     SB_PUSH_CHAR(&sb, c);
            
        //     token.line = i;
        //     token.column = 0;
        //     string_lit = true;
        //     continue;

        // } else if (string_lit) {
        //     SB_PUSH_CHAR(&sb, c);
        //     if (c == '"')  {
        //         token.kind = STRING_LIT;
        //         token.lexeme = strdup(sb.string);

        //         PARSER_PUSH_TOKEN(parser, token);
        //         SB_CLEAR(&sb);
        //         token = (Token){0};
        //         string_lit = false;
        //     }
        //     continue;
        // }
        
        // if (!number_lit && isdigit(c)) {
        //     SB_PUSH_CHAR(&sb, c);
        //     token.line = i;
        //     token.column = 0;
        //     number_lit = true;
        //     continue;
            
        // } else if (number_lit) {
        //     if (!isdigit(c))  {
        //         SB_PUSH_CHAR(&sb, '\0');
        //         token.kind = NUMBER_LIT;
        //         token.lexeme = strdup(sb.string);
                
        //         PARSER_PUSH_TOKEN(parser, token);
        //         SB_CLEAR(&sb);
        //         token = (Token){0};
        //         number_lit = false;
        //     } else { 
        //         SB_PUSH_CHAR(&sb, c);  
        //         continue;
        //     }
        // }
        
        // if (lexer_get_token(&token, c)) {
        //     token.line = i;
        //     token.column = 0;
        //     PARSER_PUSH_TOKEN(parser, token);
        //     token = (Token){0};

        //     continue;
        // }
    }
    SB_FREE(&sb);
    return true;
}

void lexer_log(Lexer lexer) {
    printf("Lexer Log:\n");
    printf("  file - %s\n", lexer.file);
    printf("  pos - %ld\n", lexer.pos);
    printf("Lexer Source:\n");
    printf("  length - %ld\n", lexer.source.length);
    printf("  bytes - %ld\n", lexer.source.size);
}