#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "core.h"
#include "ast.h"

void parse_file(const char *filepath) {
    unused(filepath);
}

int main(int argc, char **argv) {
    Lexer lexer;
    Parser parser;

    shift_arg(argc, argv);
    
    if (argc == 0) {
        fprintf(stderr, "\033[0;31mfatal error:\033[0m no input files\n");
        return 1;
    }
    
    const char *ct_file = shift_arg(argc, argv);     

    if (!lexer_init(&lexer, ct_file)) return 1;
    parser_init(&parser);

    if (!lexer_tokenize(&lexer, &parser)) return 1;

    ASTNode *it = malloc(sizeof(ASTNode));
    int iter = 0;
    while (ast_token(&parser).kind != TK_EOF) {
        ++iter;
        it = ast_parse_stmt(&parser);
        if (iter > 0) printf("\n");
        ast_log(it, 0);
    }
    printf("\n");
    // it = ast_parse_stmt(&parser)


    lexer_destroy(&lexer);
    parser_destroy(&parser);
    return 0;
}