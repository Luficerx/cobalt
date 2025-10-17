#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>

typedef struct Token Token;
typedef struct ASTNode ASTNode;

typedef struct Parser {
    Token *items;
    size_t count;
    size_t size;
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
void parser_init(Parser *parser);
void parser_destroy(Parser *parser);

void parser_log(Parser parser);
void parser_log_tokens(Parser parser);

#endif // PARSER_H