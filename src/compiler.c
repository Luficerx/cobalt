#include <stdio.h>
#include "compiler.h"

void log_compiler_version() {
    printf("cobalt version: %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}