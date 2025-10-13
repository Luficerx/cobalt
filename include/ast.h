#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef AST_H

#ifndef AST_MAX_MEM_ALLOCATABLE
    #define AST_MAX_MEM_ALLOCATABLE 8 * 1024 * 1024
#endif // AST_MAX_MEM_ALLOCATABLE

typedef int AST_INT;
typedef char * AST_STR;
typedef float AST_FLOAT;
typedef bool AST_BOOL;

typedef enum AST_Kind {
    // Primitive Types
    INT,
    STR,
    FLOAT,
    BOOL,

    // Statement Types
    // ASSIGN,
    // RETURN,
    // BREAK,
    // WHILE,
    // FOR,

} AST_Kind;

typedef struct AST_Node {
    AST_Kind kind;
    union {
        AST_STR v_str;
        AST_INT v_int;
        AST_FLOAT v_float;
        AST_BOOL v_bool;
    };
} AST_Node;

typedef struct AST_Nodes {
    AST_Node *nodes;
    size_t count;
    size_t size;
} AST_Nodes;

#endif // AST_H
#ifdef AST_IMPLENTATION

#define AST_PREP_ARRAY(nodes_array, min_size)                                                                       \
do {                                                                                                                \
    if ((min_size) > (nodes_array)->size) {                                                                         \
        if ((nodes_array)->size == 0) {                                                                             \
            (nodes_array)->size = sizeof(AST_Node);                                                                 \
        }                                                                                                           \
        while ((min_size) > (nodes_array)->size) {                                                                  \
            (nodes_array)->size *= 2;                                                                               \
        }                                                                                                           \
        (nodes_array)->nodes = realloc((nodes_array)->nodes, (nodes_array)->size * sizeof(*(nodes_array)->nodes));  \
        assert((nodes_array)->nodes != NULL);                                                                       \
        if ((nodes_array)->size > AST_MAX_MEM_ALLOCATABLE) {                                                        \
            free((nodes_array)->nodes);                                                                             \
            assert(0 && "AST_MAX_MEM_ALLOCATABLE REACHED"); }                                                       \
    }                                                                                                               \
} while (0)                                                                                                         \

#define AST_PUSH_NODE(nodes_array, node)                   \
do {                                                       \
    AST_PREP_ARRAY(nodes_array, (nodes_array)->count+1);   \
    (nodes_array)->nodes[(nodes_array)->count++] = (node); \
} while (0)                                                \

#define AST_POP_NODE_AT(nodes_array, index)                   \
do {                                                          \
    AST_FREE_NODE((nodes_array)->nodes[index]);               \
    for (size_t i = (index); i < (nodes_array)->count; ++i) { \
        (nodes_array)->nodes[i] = (nodes_array)->nodes[i+1];  \
    }                                                         \
    --(nodes_array)->count;                                   \
                                                              \
} while (0)                                                   \

#define AST_POP_NODE(nodes_array) AST_POP_NODE_AT(nodes_array, 0)
#define AST_FREE_NODE(node) if ((node).kind == STR && (node).v_str != NULL) free((node).v_str)
#define AST_FREE(nodes_array) free((nodes_array)->nodes)

void ast_log_node(AST_Node node) {
    switch (node.kind) {
        case INT: {
            printf("AST_Node: type=integer, value=%d\n", node.v_int);
            break;
        }
        case STR: {
            printf("AST_Node: type=string, value=%s\n", node.v_str);
            break;
        }
        case BOOL: {
            printf("AST_Node: type=bool, value=%d\n", node.v_bool);
            break;
        }
        case FLOAT: {
            printf("AST_Node: type=float, value=%f\n", node.v_float);
            break;
        }
    }
}

void ast_log_nodes(AST_Nodes nodes) {
    for (size_t i = 0; i < nodes.count; ++i) {
        ast_log_node(nodes.nodes[i]);
    }
}

#endif // AST_IMPLEMENTATION