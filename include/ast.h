#ifndef AST_H
#define AST_H

#include "parser.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct Parser Parser;

typedef enum ASTType {
    T_STMT,
    T_EXPR,

    T_NUMBER,
    T_BINARY_OP,
    T_IDENTIFIER,

} ASTType;

typedef struct ASTNode {
    ASTType type;
    
    struct ASTNode *expr;
    struct ASTNode *left;
    struct ASTNode *right;

    union {
        char *identifier;
        double number;
        char *op;
    } data;

} ASTNode;

// FORWARD DECLARATION
ASTNode *ast_parse_stmt(Parser *parser);
ASTNode *ast_parse_expr(Parser *parser);
ASTNode *ast_parse_term(Parser *parser);
ASTNode *ast_parse_factor(Parser *parser);

ASTNode *ast_parse_primary(Parser *parser);
ASTNode *ast_number_node(double number);
ASTNode *ast_binary_op_node(Token token, ASTNode *left, ASTNode *right);
ASTNode *ast_statement_node(ASTNode *expr);
Token ast_token(Parser *parser);

void ast_advance(Parser *parser);
void ast_log(ASTNode *node, int indent);

#endif // AST_H