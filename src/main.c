#include <stdio.h>
#include <assert.h>

#include "parser.h"
#include "lexer.h"
#include "core.h"
#include "sb.h"

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
    
    parser_log_tokens(parser);
    lexer_destroy(&lexer);
    parser_destroy(&parser);
    return 0;
}