#include <stdio.h>
#include <string.h>

#include "token.h"

char* token_kind_name(TokenKind kind) {
    switch (kind) {
        case TK_IDENTIFIER:
            return "IDENTIFIER";
        case TK_KEYWORD:
            return "KEYWORD";
        case TK_NUMBER_LIT:
            return "NUMBER";
        case TK_STRING_LIT:
            return "STRING";
        case TK_LSPAREN:
            return "LSPAREN";
        case TK_RSPAREN:
            return "RSPAREN";
        case TK_LPAREN:
            return "LPAREN";
        case TK_RPAREN:
            return "RPAREN";
        case TK_LCBRACE:
            return "LBRACE";
        case TK_RCBRACE:
            return "RBRACE";
        case TK_SEMICOLON:
            return "SEMICOLON";
        case TK_COLON:
            return "COLON";
        case TK_COMMA:
            return "COMMA";
        case TK_DOT:
            return "DOT";
        case TK_ADD:
            return "ADD";
        case TK_MINUS:
            return "MINUS";
        case TK_MULT:
            return "MULT";
        case TK_DIV:
            return "DIV";
        case TK_MOD:
            return "MOD";
        case TK_EQUALS:
            return "EQUALS";
        case TK_NEQUALS:
            return "NEQUALS";
        case TK_GREATER:
            return "GREATER";
        case TK_LESSER:
            return "LESSER";
        case TK_GEQUALS:
            return "GEQUALS";
        case TK_LEQUALS:
            return "LEQUALS";
        case TK_ASSIGN_OP:
            return "ASSIGN_OP";
        case TK_MULT_OP:
            return "MULT_OP";
        case TK_DIV_OP:
            return "DIV_OP";
        case TK_INC_OP:
            return "INC_OP";
        case TK_DEC_OP:
            return "DEC_OP";
        case TK_AND_OP:
            return "AND_OP";
        case TK_OR_OP:
            return "OR_OP";
        case TK_NOT_OP:
            return "NOT_OP";
        case TK_QMARK:
            return "QMARK";
        }
    return "UNREACHABLE";
}

void token_log(Token token) {
    char *name = token_kind_name(token.kind);
    printf("%s: %s\n", name, token.lexeme);
}

void token_kind_by_lexeme(Token *token) {
    if (strcmp(token->lexeme, "int")) token->kind = TK_KEYWORD;
    else if (strcmp(token->lexeme, "str")) token->kind = TK_KEYWORD;
    else if (strcmp(token->lexeme, "void")) token->kind = TK_KEYWORD;
}