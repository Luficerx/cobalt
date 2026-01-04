#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

typedef struct Token Token;
typedef struct ASTNode ASTNode;

typedef struct Parser {
    Token *items;
    size_t size;
    size_t len;
    size_t pos;
} Parser;

typedef enum PrecedenceOps {
    PO_POSTFIX,        // x++  x--  ()  []  .  ->
    PO_UNARY,          // ++x  --x  +x  -x  !  ~  *  &  sizeof
    PO_MUL_DIV_MOD,    // *  /  %
    PO_ADD_SUB,        // +  -

    PO_SHIFT,          // <<  >>

    PO_RELATIONAL,     // <  <=  >  >=

    PO_EQUALITY,       // ==  !=

    PO_BIT_AND,        // &
    
    PO_BIT_XOR,        // ^
    
    PO_BIT_OR,         // |
    
    PO_LOGICAL_AND,    // &&
    
    PO_LOGICAL_OR,     // ||
    
    PO_TERNARY,        // ? :
    PO_ASSIGNMENT,     // =  +=  -=  *=  /=  etc.
    
    PO_COMMA,          // ,
} PrecedenceOps;

// FORWARD DECLARATION

/*
    Set default fields for the `Parser` struct and allocates `n` bytes to `Parser.items`.
    Initially allocates `sizeof(*parser->items) * 256`.
    The array will resize as new entries are added if the limit was reached
*/
void parser_init(Parser *parser);

// Free the memory allocated in `Parser.items` and set the rest of the fields to 0.
void parser_destroy(Parser *parser);

// Print basic information about `Parser` struct.
void parser_log(Parser parser);

// Print all tokens in `Parser.items`
void parser_log_tokens(Parser parser);

// Print `n` amount of tokens in `Parser.items`
void parser_log_ntokens(Parser parser, size_t n);

#endif // PARSER_H