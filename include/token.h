#include <stddef.h>

#ifndef TOKEN_H
#define TOKEN_H

typedef enum TokenKind {
    // Literals & Identifiers
    TK_IDENTIFIER, // varname, funcname
    TK_KEYWORD,    // void, integer, string, const
    TK_NUMBER_LIT, // 69, 420
    TK_STRING_LIT, // "Hello World"
    TK_HEX_LIT,    // 0x181818 Tsoding background color theme
    
    // Delimiters
    TK_LSBRACE,   // [
    TK_RSBRACE,   // ]
    TK_LPAREN,    // (
    TK_RPAREN,    // )
    TK_LCBRACE,   // {
    TK_RCBRACE,   // }
    TK_SEMICOLON, // ;
    TK_COLON,     // :
    TK_COMMA,     // ,
    TK_DOT,       // .
    
    // Arithmetic
    TK_PLUS,      // +
    TK_MINUS,     // -
    TK_STAR,      // *
    TK_SLASH,     // /
    TK_MOD,       // %
    
    // Comparisson
    TK_GREATER,   // >
    TK_LESSER,    // <
    
    // Assignment
    TK_ASSIGN_OP, // =
    
    // Logical()
    TK_EQUALS,    // ==
    TK_NEQUALS,   // !=
    TK_GEQUALS,   // >=
    TK_LEQUALS,   // <=
    TK_MULT_OP,   // *=
    TK_DIV_OP,    // /=
    TK_AND_OP,    // &&
    TK_OR_OP,     // ||
    
    TK_QMARK,     // ?
    TK_NOT,       // !

    // OTHER
    TK_AT,        // @
    TK_HASH,      // #
    TK_DSIGN,     // $
    TK_AMPER,     // &

    // SPECIAL
    TK_INC_OP,    // ++
    TK_DEC_OP,    // --

} TokenKind;

typedef enum TokenMode {
    TM_NONE = 0,
    TM_STRING_LIT,
    TM_NUMBER_LIT,
    TM_HEX_LIT,
    TM_MCOMMENT, // MULTI COMMENT
    TM_COMMENT,  // SINGLE COMMENT
    TM_GENERIC,
} TokenMode;

typedef struct Token {
    TokenKind kind;
    char *lexeme;
    size_t line;
    size_t column;
} Token;

// FORWARD DECLARATION

char* token_kind_name(TokenKind kind);
void token_log(Token token);
#endif // TOKEN_H