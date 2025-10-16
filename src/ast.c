#include <stdio.h>

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "parser.h"
#include "token.h"
#include "core.h"
#include "ast.h"

ASTNode *ast_parse_stmt(Parser *parser) {
    ASTNode *expr = ast_parse_expr(parser);
    Token token = ast_token(parser);

    if (token.kind != TK_SEMICOLON) {
        fprintf(stderr, "error: expected "CORE_RED"';'"CORE_END" but got "CORE_RED"'%s'"CORE_END"\n", token.lexeme);
        exit(1);
    }
    
    ast_advance(parser);

    return ast_statement_node(expr);
}

ASTNode *ast_parse_expr(Parser *parser) {
    ASTNode *left = ast_parse_term(parser);

    while (ast_token(parser).kind == TK_ADD || ast_token(parser).kind == TK_SUB) {
        Token token = ast_token(parser);
        ast_advance(parser);
        ASTNode *right = ast_parse_term(parser);
        left = ast_binary_op_node(token, left, right);
    }
    
    return left;
}

ASTNode *ast_parse_term(Parser *parser) {
    ASTNode *left = ast_parse_factor(parser);
    while (ast_token(parser).kind == TK_STAR || ast_token(parser).kind == TK_SLASH ) {
        Token token = ast_token(parser);
        ast_advance(parser);
        ASTNode *right = ast_parse_term(parser);
        left = ast_binary_op_node(token, left, right);
    }
    return left;
}

ASTNode *ast_parse_factor(Parser *parser) {
    Token token = ast_token(parser);
    
    if (token.kind == TK_NUMBER_LIT) {
        double number = atof(token.lexeme);
        ast_advance(parser);
        return ast_number_node(number);
    }

    if (token.kind == TK_LPAREN) {
        ast_advance(parser);
        ASTNode *expr = ast_parse_expr(parser);
        if (ast_token(parser).kind != TK_RPAREN) {
            fprintf(stderr, "error: expected closing paren "CORE_RED"')'"CORE_END".\n");
            exit(1);
        }
        ast_advance(parser);
    return expr;
    }

    fprintf(stderr, "\n"CORE_RED"error: "CORE_END"unknown symbol:"CORE_RED" '%s' "CORE_END"\n", ast_token(parser).lexeme);
    exit(1);
}

// ASTNode *ast_parse_primary(Parser *parser) {
//     if (ast_token(parser).kind == TK_NUMBER_LIT) {
//         Token token = ast_token(parser);
//         double number = atof(token.lexeme);
//         ast_advance(parser);
//         return ast_number_node(number);
//     }
// }

ASTNode *ast_number_node(double number) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_NUMBER;
    node->data.number = number;
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode *ast_binary_op_node(Token token, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_BINARY_OP;
    node->data.op = token.lexeme;
    node->left = left;
    node->right = right;
    return node;
}

ASTNode *ast_statement_node(ASTNode *expr) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_STMT;
    node->expr = expr;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Token ast_token(Parser *parser) { return parser->tokens[parser->pos]; }

void ast_advance(Parser *parser) {
    parser->pos++;
    if (parser->pos > parser->count) assert(0 && "Parser position greater than amount of tokens present");
}

void ast_log(ASTNode *node, int indent) {
    if (node == NULL) return;
    
    // Print indentation
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    // Print the node based on its type
    switch (node->type) {
        case T_NUMBER:
            printf("NUMBER: %.2f\n", node->data.number);
            break;
            
        case T_BINARY_OP:
            printf("BINARY_OP: %s\n", node->data.op);
            ast_log(node->left, indent + 1);   // Print left child
            ast_log(node->right, indent + 1);  // Print right child
            break;
            
        case T_IDENTIFIER:
            printf("IDENTIFIER: %s\n", node->data.identifier);
            break;

        case T_EXPR:
            printf("EXPR\n");
            // Print the expression if it has one
            if (node->expr) {
                ast_log(node->expr, indent + 1);
            }
            break;

        case T_STMT:
            printf("STMT\n");  // Fixed: don't print data.op
            // Print the expression inside the statement
            if (node->expr) {
                ast_log(node->expr, indent + 1);
            }
            break;
    }
}
