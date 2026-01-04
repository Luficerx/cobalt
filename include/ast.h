#ifndef AST_H
#define AST_H

typedef struct ASTNode {
    struct ASTNode *l;    // Left side of expression
    struct ASTNode *r;    // Right side of expression
} ASTNode;

// Allocates a storage for `ASTNode`'s.
void ast_init();

// Free the allocated storage.
void ast_destroy();

/*
    Creates a new node and returns a pointer to it.
    Both `*l` and `*r` can be `NULL` to represent the initial node of the tree.
*/
ASTNode *ast_node(ASTNode *l, ASTNode *r);

// Akin to `ast_node` returns a pointer to a node whose fields are set to null.
ASTNode *ast_null();

#endif // AST_H