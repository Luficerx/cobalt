#include <stdio.h>
#include <stdbool.h>

#include "parser.h"
#include "token.h"
#include "core.h"

void parser_init(Parser *parser) {
    parser->items = NULL;
    parser->len = 0;
    parser->size = 0;
    parser->pos = 0;
}

void parser_destroy(Parser *parser) { (void)parser; }

void parser_log(Parser parser) {
    printf("Parser Log:\n");
    printf("  len - %ld\n", parser.len);
    printf("  size - %ld\n", parser.size);
    printf("  pos - %ld\n", parser.pos);
}

void parser_log_tokens(Parser parser) {
    size_t n = parser.len > 16 ? 16 : parser.len;

    printf("Tokens found: %ld\n", parser.len);
    for (size_t i = 0; i < n; ++i) {
        printf(CORE_DARK_GREY"...."CORE_END);
        token_log(parser.items[i]);
    }
}
