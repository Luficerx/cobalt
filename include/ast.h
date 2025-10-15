#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef AST_H
#define AST_H

typedef struct ASTNode {
    char *value;
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

#endif // AST_H