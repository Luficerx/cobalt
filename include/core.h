#ifndef CORE_H
#define CORE_H

#define CORE_RED "\x1b[38;2;186;14;46m"
#define CORE_LIGHT_GREY "\x1b[38;2;32;32;32m"
#define CORE_DARK_GREY "\x1b[38;2;64;64;64m"
#define CORE_END "\x1b[0m"

// Cast to void.
#define DEAD(t) (void)t

#define NOIMPL()                                                        \
do {                                                                    \
    fprintf(stderr,                                                     \
            "%s: %d"CORE_RED" %s(...) "CORE_END"is not implemented.\n", \
            __FILE__, __LINE__, __func__);                              \
    exit(1);                                                            \
} while (0)

#define THROW(message)                        \
do {                                          \
    fprintf(stderr,                           \
            "%s: %d"CORE_RED" %s.\n"CORE_END, \
            __FILE__, __LINE__, message);     \
    exit(1);                                  \
} while (0)

#define ERROR(message)                                       \
do {                                                         \
    fprintf(stderr,                                          \
            "%s: %d"CORE_RED" fatal error: "CORE_END"%s.\n", \
            __FILE__, __LINE__, message);                    \
} while (0)
#endif // CORE_H