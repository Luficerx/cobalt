#include <stdio.h>
#include <stdbool.h>

#include "parser.h"
#include "token.h"
#include "ast.h"

void parser_init(Parser *parser) {
    parser->items = NULL;
    parser->count = 0;
    parser->size = 0;
    parser->pos = 0;
}

void parser_destroy(Parser *parser) { (void)parser; }

void parser_log(Parser parser) {
    printf("Parser Log:\n");
    printf("  count - %ld\n", parser.count);
    printf("  size - %ld\n", parser.size);
    printf("  pos - %ld\n", parser.pos);
}

void parser_log_tokens(Parser parser) {
    printf("Tokens found: %ld\n", parser.count);
    for (size_t i = 0; i < parser.count; ++i) {
        printf("    ");
        token_log(parser.items[i]);
    }
}