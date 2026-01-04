#define ARRAY_STRIP_PREFIX
#include "array.h"

#include "compiler.h"
#include "parser.h"
#include "token.h"
#include "lexer.h"
#include "core.h"

int main(int argc, char *argv[]) {
    Lexer lexer;
    Parser parser;

    shift_arg(argc, argv);

    if (argc == 0) { ERROR("No input files."); return 1; }

    if (!compiler_resolve_flags(argc, argv)) return 1;

    const char *ct_file = shift_arg(argc, argv);

    if (!lexer_init(&lexer, ct_file)) return 1;
    parser_init(&parser);

    if (!lexer_tokenize(&lexer, &parser)) return 1;

    parser_log_tokens(parser);

    lexer_destroy(&lexer);
    parser_destroy(&parser);
    return 0;
}