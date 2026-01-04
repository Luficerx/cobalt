#include "ast.h"
#include "arena.h"

Arena nodes_arena = {0};

void ast_init() {
    arena_init(&nodes_arena);
}

void ast_destroy() {
    arena_destroy(&nodes_arena);
}

ASTNode *ast_node(ASTNode *l, ASTNode *r) { 
    ASTNode *node = arena_new(&nodes_arena, ASTNode);
    node->l = l;
    node->r = r;
    return node;
}

ASTNode *ast_null() { return ast_node(NULL, NULL); }