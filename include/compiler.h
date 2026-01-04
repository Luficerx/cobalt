#ifndef COMPILER_H
#define COMPILER_H

#include <stdbool.h>

#define VERSION 0x313030
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0

// Print the current compiler version.
void log_compiler_version(void);

// Resolve all flags passed through `main` function.
bool compiler_resolve_flags(int argc, char **argv);

#endif //COMPILER_H