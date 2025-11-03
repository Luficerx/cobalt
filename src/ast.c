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
        // int x;
        // ^~~
        
        if (ast_next_token(parser).kind == TK_IDENTIFIER) {
            ASTNode *base_type = ast_identifier_node(NULL, token.lexeme);

            ast_advance(parser, __LINE__);
            
            token = ast_token(parser);
            ASTNode *node = ast_identifier_node(base_type, token.lexeme);
            
            ast_advance(parser, __LINE__);
            
            if (ast_token(parser).kind == TK_ASSIGN_OP) {
                ast_advance(parser, __LINE__);

                ASTNode *expr = ast_parse_expr(parser);
                ASTNode *decl = ast_declaration_node(node, expr, expr->value);
                
                ast_expect(ast_token(parser), TK_SEMICOLON, ";", __LINE__);
                ast_advance(parser, __LINE__);
                
                return decl;
            }

            // if (ast_token(parser).kind == TK_COMMA) {
                 // Multi declaration
                 // int x, y, z;


            // }

            ast_expect(ast_token(parser), TK_SEMICOLON, ";", __LINE__);
            ast_advance(parser, __LINE__);
            ASTNode *decl = ast_declaration_node(NULL, NULL, node);
            return decl;
        }

        if (ast_next_token(parser).kind == TK_ASSIGN_OP) {
            // Assignment
            // x = 1;

            ASTNode *left = ast_parse_assignment(parser);

            ast_advance(parser, __LINE__);
            ASTNode *right = ast_parse_expr(parser);

            token = ast_token(parser);
            ast_expect(token, TK_SEMICOLON, ";", __LINE__);
            ast_advance(parser, __LINE__);
            return ast_assignment_node(left, right);
        }

        if (ast_next_token(parser).kind == TK_LPAREN) {
            ASTNode *call = ast_parse_call(parser);
            ast_expect(ast_token(parser), TK_SEMICOLON, ";", __LINE__);
            ast_advance(parser, __LINE__);
            return call;
        }

        // ast_expect(ast_next_token(parser), TK_SEMICOLON, ";", __LINE__);
        // fprintf(stderr, "error: Invalid value: %s : %d\n", ast_next_token(parser).lexeme, __LINE__);
        // exit(1);
    }
    
    // Expression
    
    ASTNode *expr = ast_parse_expr(parser);
    token = ast_token(parser);
    ast_expect(token, TK_SEMICOLON, ";", __LINE__);
    ast_advance(parser, __LINE__);

    return ast_statement_node(expr);
}

ASTNode *ast_parse_declaration(Parser *parser) {
    (void)parser;
    return NULL;
}

ASTNode *ast_parse_call(Parser *parser) {
    ast_advance(parser, __LINE__);
    
    Token token = ast_token(parser);
    
    if (token.kind == TK_LPAREN) {
        ast_advance(parser, __LINE__);
        if (ast_token(parser).kind == TK_RPAREN) {
            ASTNode *expr = ast_empty_expr_node();
            ast_advance(parser, __LINE__);
            return expr;
        }
        
        ASTNode *expr = ast_parse_expr(parser);

        if (ast_token(parser).kind != TK_RPAREN) {
            fprintf(stderr, "error: expected closing paren "CORE_RED"')'"CORE_END".%d\n", __LINE__);
            exit(1);
        }

        ast_advance(parser, __LINE__);
        return expr;
    }
    
    // Unknown for now
    ASTNode *expr = ast_parse_expr(parser);
    token = ast_token(parser);
    ast_expect(token, TK_SEMICOLON, ";", __LINE__);
    ast_advance(parser, __LINE__);

    return ast_statement_node(expr);
}

ASTNode *ast_parse_multi_declaration(Parser *parser) {
    ASTNodes *declarators = ast_new_declarators();
    
    Token type_token = ast_token(parser);
    ASTNode *type_node = ast_identifier_node(NULL, type_token.lexeme);
    ASTNode *decl = ast_multi_declaration_node(type_node, NULL, NULL, declarators);
    
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
            continue;
        };

        fprintf(stderr, "ast_parse_multi_declaration");
        fprintf(stderr, "\n"CORE_RED"error: "CORE_END"unknown symbol:"CORE_RED" '%s' "CORE_END" : %d\n", ast_token(parser).lexeme, __LINE__);
        exit(1);
    }

    return decl;
}

void ast_parse_declarator(ASTNodes *declarators, Parser *parser) {
    Token token = ast_token(parser);
    ASTNode *declarator = ast_identifier_node(NULL, token.lexeme);
    DA_APPEND(declarators, *declarator);
    ast_advance(parser, __LINE__);
}

ASTNode *ast_parse_assignment(Parser *parser) {
    Token token = ast_token(parser);
    ast_advance(parser, __LINE__);
    return ast_identifier_node(NULL, token.lexeme);
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

    if (token.kind == TK_STRING_LIT) {
        ast_advance(parser, __LINE__);
        return ast_string_node(token.lexeme);
    }

    if (token.kind == TK_IDENTIFIER) {
        ast_advance(parser, __LINE__);
        return ast_identifier_node(NULL, token.lexeme);
    }

    if (token.kind == TK_HEX_LIT) {
        size_t number = atof(token.lexeme);
        ast_advance(parser, __LINE__);
        return ast_hexadecimal_node(number);
    }

    if (token.kind == TK_LPAREN) {
        ast_advance(parser, __LINE__);
        if (ast_token(parser).kind == TK_RPAREN) {
            ASTNode *expr = ast_empty_expr_node();
            ast_advance(parser, __LINE__);
            return expr;
        }
        
        ASTNode *expr = ast_parse_expr(parser);
        if (ast_token(parser).kind != TK_RPAREN) {
            fprintf(stderr, "error: expected closing paren "CORE_RED"')'"CORE_END".%d\n", __LINE__);
            exit(1);
        }
        ast_advance(parser, __LINE__);
        return expr;
    }

    fprintf(stderr, "ast_parse_factor");
    fprintf(stderr, "\n"CORE_RED"error: "CORE_END"unknown symbol:"CORE_RED" '%s' "CORE_END" : %d\n", ast_token(parser).lexeme, __LINE__);
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

ASTNode *ast_declaration_node(ASTNode *left, ASTNode *right, ASTNode *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_DECLARATION;

    node->base_type = NULL;
    node->declarators = NULL;

    node->expr = NULL;
    node->left = left;
    node->right = right;

    node->value = value;

    return node;
}

ASTNode *ast_multi_declaration_node(ASTNode *base_type, ASTNode *left, ASTNode *right, ASTNodes *declarators) {
    ASTNode *node = malloc(sizeof(ASTNode));
    (void)base_type;
    (void)left;
    (void)right;
    (void)declarators;

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

ASTNode *ast_identifier_node(ASTNode *base_type, char *identifier) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_IDENTIFIER;
    node->base_type = base_type;

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
    
    node->raw_value.number = number;

    return node;
}

ASTNode *ast_number_node(int number) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_NUMBER;
    
    node->left = NULL;
    node->right = NULL;
    
    node->raw_value.number = number;

    return node;
}

ASTNode *ast_string_node(char *string) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_STRING;
    
    node->left = NULL;
    node->right = NULL;
    
    node->raw_value.string = string;

    return node;
}

ASTNode *ast_empty_expr_node() {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = T_EXPR;
    
    node->base_type = NULL;
    node->declarators = NULL;

    node->left = NULL;
    node->right = NULL;
    
    node->value = NULL;

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

void ast_expect(Token token, TokenKind target, char *lexeme, int line) {
    if (token.kind != target) {
        #ifdef COMPILER_FUNCTION_ERROR
        fprintf(stderr, "Failed at 'ast_expect' at line: %d\n", line);
        #endif
        fprintf(stderr, "error: expected "CORE_RED"'%s'"CORE_END" but got "CORE_RED"'%s'"CORE_END" at %ld:%ld\n", lexeme, token.lexeme, token.column, token.line);
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

void ast_log_indent(int indent) {
    for (int i = 0; i < indent; i++) printf("  ");
}

void ast_log(ASTNode *node, int indent) {
    if (node == NULL) return;
    
    ast_log_indent(indent);
    
    // Print the node based on its type
    switch (node->type) {
        case T_CALL:
            printf("CALL: %d\n", node->raw_value.number);
            break;

        case T_NUMBER:
            printf("NUMBER: %d\n", node->raw_value.number);
            break;

        case T_STRING:
            printf("STRING: %s\n", node->raw_value.string);
            break;

        case T_HEXADECIMAL:
            printf("NUMBER: %X\n", node->raw_value.number);
            break;
            
        case T_BINARY_OP:
            printf("BINARY_OP: %s\n", node->data.op);
            ast_log(node->left, indent + 1);   // Print left child
            ast_log(node->right, indent + 1);  // Print right child
            break;
            
        case T_IDENTIFIER:
            printf("IDENTIFIER: %s\n", node->data.identifier);
            if (node->base_type) ast_log(node->base_type, indent + 1);
            break;
            
        case T_EXPR:
            printf("EXPR\n");
            // Print the expression if it has one
            if (node->expr) {
                ast_log(node->expr, indent + 1);
            }
            break;
            
        case T_MDECLARATION:
            printf("COMPOSED_DECL:\n");
            if (node->left) ast_log(node->left, indent + 1);
            if (node->right) ast_log(node->right, indent + 1);
            break;
            
        case T_DECLARATION:
            printf("DECLARATION:\n");
            if (node->value) ast_log(node->value, indent + 1);
            if (node->left) ast_log(node->left, indent + 1);
            if (node->right) ast_log(node->right, indent + 1);
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
