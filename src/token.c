#include <stdio.h>

#include "token.h"
#include "core.h"

char* token_kind_name(TokenKind kind) {
    switch (kind) {
        case TK_IDENTIFIER: return "IDENTIFIER";
        case TK_KEYWORD:    return "KEYWORD";
        case TK_NUMBER_LIT: return "NUMBER_LIT";
        case TK_STRING_LIT: return "STRING_LIT";
        case TK_HEX_LIT:    return "HEX_LIT";
        case TK_LSBRACE:    return "LSBRACE";
        case TK_RSBRACE:    return "RSBRACE";
        case TK_LPAREN:     return "LPAREN";
        case TK_RPAREN:     return "RPAREN";
        case TK_LCBRACE:    return "LCBRACE";
        case TK_RCBRACE:    return "RCBRACE";
        case TK_SEMICOLON:  return "SEMICOLON";
        case TK_COLON:      return "COLON";
        case TK_PIPE:       return "COLON";
        case TK_COMMA:      return "COMMA";
        case TK_DOT:        return "DOT";
        case TK_ADD:        return "PLUS";
        case TK_SUB:        return "MINUS";
        case TK_STAR:       return "STAR";
        case TK_SLASH:      return "SLASH";
        case TK_MOD:        return "MOD";
        case TK_RSHIFT:     return "RSHIFT";
        case TK_LSHIFT:     return "LSHIFT";
        case TK_GREATER:    return "GREATER";
        case TK_LESSER:     return "LESSER";
        case TK_ASSIGN_OP:  return "ASSIGN_OP";
        case TK_EQUALS:     return "EQUALS";
        case TK_NEQUALS:    return "NEQUALS";
        case TK_GEQUALS:    return "GEQUALS";
        case TK_LEQUALS:    return "LEQUALS";
        case TK_MULT_OP:    return "MULT_OP";
        case TK_DIV_OP:     return "DIV_OP";
        case TK_AND_OP:     return "AND_OP";
        case TK_OR_OP:      return "OR_OP";
        case TK_QMARK:      return "QMARK";
        case TK_NOT:        return "NOT";
        case TK_AT:         return "AT";
        case TK_HASH:       return "HASH";
        case TK_DSIGN:      return "DSIGN";
        case TK_AMPER:      return "AMPER";
        case TK_XOR:        return "XOR";
        case TK_INC_OP:     return "INC_OP";
        case TK_DEC_OP:     return "DEC_OP";
        case TK_EOF:        return "EOF";
    }
    return "UNREACHABLE";
}

void token_log(Token token) {
    const char *name = token_kind_name(token.kind);
    printf("%s → "CORE_RED"%s"CORE_END"\n", name, token.lexeme);
}