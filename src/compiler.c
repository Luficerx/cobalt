#include <stdio.h>
#include <stdbool.h>

#include "compiler.h"
#include "core.h"

void log_compiler_version() {
    printf("cobalt version: %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}

bool compiler_resolve_flags(int argc, char **argv) {
    DEAD(argc);
    DEAD(argv);

    return true;
}