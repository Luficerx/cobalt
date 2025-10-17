#include <stdio.h>

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "parser.h"
#include "token.h"
#include "core.h"
#include "ast.h"
#include "da.h"

ASTNode *ast_parse_stmt(Parser *parser) {
    Token token = ast_token(parser);
    
    if (token.kind == TK_IDENTIFIER) {
        if (ast_next_token(parser).kind == TK_ASSIGN_OP) {
            ASTNode *left = ast_parse_assignment(parser);

            ast_advance(parser, __LINE__);
            ASTNode *right = ast_parse_expr(parser);

            token = ast_token(parser);
            ast_expect(token, TK_SEMICOLON, ";");
            ast_advance(parser, __LINE__);
            return ast_assignment_node(left, right);
        }

        if (ast_next_token(parser).kind == TK_IDENTIFIER) {
            ASTNode *decl = ast_parse_declaration(parser);
            ast_advance(parser, __LINE__);
            return decl;
        }

        fprintf(stderr, "error: Invalid value: %s\n", ast_token(parser).lexeme);
        exit(1);
    }
    
    // Expression
    
    ASTNode *expr = ast_parse_expr(parser);
    token = ast_token(parser);
    ast_expect(token, TK_SEMICOLON, ";");
    ast_advance(parser, __LINE__);

    return ast_statement_node(expr);
}

ASTNode *ast_parse_declaration(Parser *parser) {
    ASTNodes *declarators = ast_new_declarators();
    
    Token type_token = ast_token(parser);
    ASTNode *type_node = ast_identifier_node(type_token.lexeme);
    
    ASTNode *decl = ast_declaration_node(type_node, NULL, NULL, declarators);

    ast_advance(parser, __LINE__);

    while (ast_token(parser).kind == TK_IDENTIFIER) {
        ast_parse_declarator(declarators, parser);

        if (ast_token(parser).kind == TK_COMMA) {
            ast_advance(parser, __LINE__);
            continue;
        }

        if (ast_token(parser).kind == TK_SEMICOLON) break;

        if (ast_token(parser).kind == TK_ASSIGN_OP) {
            ast_advance(parser, __LINE__);
            ASTNode *value = ast_parse_expr(parser);
            decl->value = value;
            break;
        };

        fprintf(stderr, "\n"CORE_RED"error: "CORE_END"unknown symbol:"CORE_RED" '%s' "CORE_END"\n", ast_token(parser).lexeme);
        exit(1);
    }

    return decl;
}

void ast_parse_declarator(ASTNodes *declarators, Parser *parser) {
    Token token = ast_token(parser);
    ASTNode *declarator = ast_identifier_node(token.lexeme);
    DA_APPEND(declarators, *declarator);
    ast_advance(parser, __LINE__);
}

ASTNode *ast_parse_assignment(Parser *parser) {
    Token token = ast_token(parser);
    ast_advance(parser, __LINE__);
    return ast_identifier_node(token.lexeme);
}

ASTNode *ast_parse_expr(Parser *parser) {
    ASTNode *left = ast_parse_term(parser);

    while (ast_token(parser).kind == TK_ADD || ast_token(parser).kind == TK_SUB) {
        Token token = ast_token(parser);
        ast_advance(parser, __LINE__);
        ASTNode *right = ast_parse_term(parser);
        left = ast_binary_op_node(token, left, right);
    }
    
    return left;
}

ASTNode *ast_parse_term(Parser *parser) {
    ASTNode *left = ast_parse_factor(parser);

    while (ast_token(parser).kind == TK_STAR || ast_token(parser).kind == TK_SLASH ) {
        Token token = ast_token(parser);
        ast_advance(parser, __LINE__);
        ASTNode *right = ast_parse_term(parser);
        left = ast_binary_op_node(token, left, right);
    }

    return left;
}

ASTNode *ast_parse_factor(Parser *parser) {
    Token token = ast_token(parser);

    if (token.kind == TK_NUMBER_LIT) {
        size_t number = atof(token.lexeme);
        ast_advance(parser, __LINE__);
        return ast_number_node(number);
    }

    if (token.kind == TK_IDENTIFIER) {
        ast_advance(parser, __LINE__);
        return ast_identifier_node(token.lexeme);
    }

    if (token.kind == TK_HEX_LIT) {
        size_t number = atof(token.lexeme);
        ast_advance(parser, __LINE__);
        return ast_hexadecimal_node(number);
    }

    if (token.kind == TK_LPAREN) {
        ast_advance(parser, __LINE__);
        ASTNode *expr = ast_parse_expr(parser);
        if (ast_token(parser).kind != TK_RPAREN) {
            fprintf(stderr, "error: expected closing paren "CORE_RED"')'"CORE_END".\n");
            exit(1);
        }
        ast_advance(parser, __LINE__);
        return expr;
    }

    fprintf(stderr, "\n"CORE_RED"error: "CORE_END"unknown symbol:"CORE_RED" '%s' "CORE_END"\n", ast_token(parser).lexeme);
    exit(1);
}

ASTNode *ast_statement_node(ASTNode *expr) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_STMT;

    node->expr = expr;
    node->left = NULL;
    node->right = NULL;

    return node;
}

ASTNode *ast_declaration_node(ASTNode *base_type, ASTNode *left, ASTNode *right, ASTNodes *declarators) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_DECLARATION;

    node->base_type = base_type;
    node->declarators = declarators;

    node->expr = NULL;
    node->left = left;
    node->right = right;

    return node;
}

ASTNode *ast_assignment_node(ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_ASSIGNMENT;

    node->expr = NULL;
    node->left = left;
    node->right = right;

    node->data.op = "=";

    return node;
}

ASTNode *ast_binary_op_node(Token token, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_BINARY_OP;
    
    node->left = left;
    node->right = right;

    node->data.op = token.lexeme;

    return node;
}

ASTNode *ast_identifier_node(char *identifier) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_IDENTIFIER;
    
    node->left = NULL;
    node->right = NULL;
    
    node->data.identifier = identifier;

    return node;
}

ASTNode *ast_hexadecimal_node(int number) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_HEXADECIMAL;
    
    node->left = NULL;
    node->right = NULL;
    
    node->data.number = number;

    return node;
}

ASTNode *ast_number_node(int number) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_NUMBER;
    
    node->left = NULL;
    node->right = NULL;
    
    node->data.number = number;

    return node;
}

// ASTNode *ast_parse_primary(Parser *parser) {
    //     if (ast_token(parser).kind == TK_NUMBER_LIT) {
//         Token token = ast_token(parser);
//         double number = atof(token.lexeme);
//         ast_advance(parser, __LINE__);
//         return ast_number_node(number);
//     }
// }

ASTNodes *ast_new_declarators() {
    ASTNodes *declarators = malloc(sizeof(ASTNodes));
    
    declarators->items = NULL;
    declarators->count = 0;
    declarators->size = 0;

    return declarators;
}

Token ast_token(Parser *parser) { return parser->items[parser->pos]; }
Token ast_next_token(Parser *parser) { return parser->items[parser->pos+1]; }

void ast_expect(Token token, TokenKind target, char *lexeme) {
    if (token.kind != target) {
        fprintf(stderr, "error: expected "CORE_RED"'%s'"CORE_END" but got "CORE_RED"'%s'"CORE_END"\n", lexeme, token.lexeme);
        exit(1);
    }
}

void ast_advance(Parser *parser, int line) {
    parser->pos++;
    if (parser->pos > parser->count) {
        fprintf(stderr, CORE_RED"error:"CORE_END" parser position greater than amount of tokens present: %s, %d\n", __FILE__, line);
        exit(1);
    }
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
            printf("NUMBER: %d\n", node->data.number);
            break;

        case T_HEXADECIMAL:
            printf("NUMBER: %X\n", node->data.number);
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
            
        case T_COMPOSED_DECL:
            printf("COMPOSED_DECL:\n");
            if (node->left) ast_log(node->left, indent + 1);
            if (node->right) ast_log(node->right, indent + 1);
            break;

        case T_DECLARATION:
            printf("DECLARATION:\n");
            ast_log(node->base_type, indent + 1);
            if (node->value) {
                ast_log(node->value, indent+1);
            }
            ast_log_declarators(node->declarators, indent+1);
            break;

        case T_ASSIGNMENT:
            printf("ASSIGNMENT:\n");
            if (node->left) ast_log(node->left, indent + 1);
            if (node->right) ast_log(node->right, indent + 1);
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

void ast_log_declarators(ASTNodes *declarators, int indent) {
    printf("  DECLARATORS: %ld\n", declarators->count);
    for (size_t i = 0; i < declarators->count; ++i) {
        ast_log(&declarators->items[i], indent+1);
    }
}
