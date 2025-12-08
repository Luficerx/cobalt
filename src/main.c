#include <stdio.h>
#include <assert.h>

#define ARRAY_STRIP_PREFIX
#include "array.h"

#include "compiler.h"
#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "core.h"

int main(int argc, char **argv) {
    Lexer lexer;
    Parser parser;

    shift_arg(argc, argv);
    
    if (argc == 0) {
        fprintf(stderr, CORE_RED"fatal error: "CORE_END"no input files\n");
        return 1;
    }
    
    printf("──────────────────────────────\n");
    
    log_compiler_version();

    printf("──────────────────────────────\n");

    const char *ct_file = array_shift_arg(argc, argv);

    if (!lexer_init(&lexer, ct_file)) return 1;
    parser_init(&parser);

    if (!lexer_tokenize(&lexer, &parser)) return 1;

    parser_log_tokens(parser);
    
    printf("──────────────────────────────\n");

    // ASTNode *it;
    // while (ast_token(&parser).kind != TK_EOF) {
    //     if (parser.pos > 0) printf("\n");
    //     it = ast_parse_stmt(&parser);
    //     ast_log(it, 0);
    // }

    // printf("──────────────────────────────\n");

    // lexer_destroy(&lexer);
    // parser_destroy(&parser);
    return 0;
}