#include <stddef.h>

#ifndef TOKEN_H
#define TOKEN_H
// #ifndef TOKEN_MAX_MEM_ALLOCATABLE
//     #define TOKEN_MAX_MEM_ALLOCATABLE 8 * 1024 * 1024
// #endif // TOKEN_MAX_MEM_ALLOCATABLE

typedef enum TokenKind {
    // Literals & Identifiers
    TK_IDENTIFIER,   // var_name, func_name
    TK_KEYWORD,   // void, integer, string, const
    TK_NUMBER_LIT,    // 69, 420
    TK_STRING_LIT,    // "Hello World"
    
    // Delimiters
    TK_LSPAREN,   // [
    TK_RSPAREN,   // ]
    TK_LPAREN,    // (
    TK_RPAREN,    // )
    TK_LCBRACE,   // {
    TK_RCBRACE,   // }
    TK_SEMICOLON, // ;
    TK_COLON,     // :
    TK_COMMA,     // ,
    TK_DOT,       // .

    // Arithmetic
    TK_ADD,       // +
    TK_MINUS,     // -
    TK_MULT,      // *
    TK_DIV,       // /
    TK_MOD,       // %

    // Comparisson
    TK_EQUALS,    // ==
    TK_NEQUALS,   // !=
    TK_GREATER,   // >
    TK_LESSER,    // <
    TK_GEQUALS,   // >=
    TK_LEQUALS,   // <=

    // Assignment
    TK_ASSIGN_OP, // =
    TK_MULT_OP,   // *=
    TK_DIV_OP,    // /=
    TK_INC_OP,    // ++
    TK_DEC_OP,    // --

    // Logical()
    TK_AND_OP,    // &&
    TK_OR_OP,     // ||
    TK_NOT_OP,    // !
    TK_QMARK,     // ?

} TokenKind;

typedef enum TokenMode {
    TM_STRING_LIT,
    TM_NUMBER_LIT,
    TM_GENERIC
} TokenMode;

typedef struct Token {
    TokenKind kind;
    TokenMode mode;
    char *lexeme;
    size_t line;
    size_t column;
} Token;

// FORWARD DECLARATION
void token_log(Token token);
char* token_kind_name(TokenKind kind);
void token_kind_by_lexeme(Token *token);
#endif // TOKEN_H