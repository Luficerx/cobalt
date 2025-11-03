#ifndef AST_H
#define AST_H

#include "parser.h"
#include <stddef.h>
#include <stdbool.h>

#define COMPILER_FUNCTION_ERROR

typedef struct Parser Parser;
typedef enum TokenKind TokenKind;

typedef enum ASTType {
    T_STMT,
    T_EXPR,
    T_CALL,

    T_MDECLARATION,
    T_DECLARATION,
    T_ASSIGNMENT,

    T_BINARY_OP,

    T_NUMBER,
    T_STRING,
    T_HEXADECIMAL,
    T_IDENTIFIER,

} ASTType;

typedef struct ASTNodes {
    ASTNode *items;
    size_t count;
    size_t size;
} ASTNodes;

typedef struct ASTNode {
    ASTType type;
    
    ASTNode *base_type;
    ASTNodes *declarators;
    
    struct ASTNode *expr;
    struct ASTNode *left;
    struct ASTNode *right;

    union {
        char *identifier;
        char *op;
    } data;

    union {
        char *string;
        int number;
    } raw_value;

    ASTNode *value;

} ASTNode;

// FORWARD DECLARATION

// Parsers
ASTNode *ast_parse_stmt(Parser *parser);
ASTNode *ast_parse_expr(Parser *parser);
ASTNode *ast_parse_term(Parser *parser);
ASTNode *ast_parse_factor(Parser *parser);
ASTNode *ast_parse_empty(Parser *parser);

ASTNode *ast_parse_declaration(Parser *parser);
ASTNode *ast_parse_call(Parser *parser);
void ast_parse_declarator(ASTNodes *declarators, Parser *parser);
ASTNode *ast_parse_assignment(Parser *parser);


// Nodes

ASTNode *ast_statement_node(ASTNode *expr);
ASTNode *ast_declaration_node(ASTNode *left, ASTNode *right, ASTNode *value);
ASTNode *ast_multi_declaration_node(ASTNode *base_type, ASTNode *left, ASTNode *right, ASTNodes *declarators);
ASTNode *ast_assignment_node(ASTNode *left, ASTNode *right);

ASTNode *ast_binary_op_node(Token token, ASTNode *left, ASTNode *right);

ASTNode *ast_identifier_node(ASTNode *base_type, char *identifier);
ASTNode *ast_hexadecimal_node(int number);
ASTNode *ast_number_node(int number);
ASTNode *ast_string_node(char *string);
ASTNode *ast_empty_expr_node();

ASTNode *ast_parse_primary(Parser *parser);
ASTNodes *ast_new_declarators();

Token ast_token(Parser *parser);
Token ast_next_token(Parser *parser);
void ast_expect(Token token, TokenKind target, char *lexeme, int line);
void ast_advance(Parser *parser, int line);
void ast_log(ASTNode *node, int indent);
void ast_log_declarators(ASTNodes *declarators, int indent);
#endif // AST_H