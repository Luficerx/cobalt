#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef SB_H
#define SB_H

#ifndef SB_DEFAULT_SIZE
    #define SB_DEFAULT_SIZE 512
#endif //SB_DEFAULT_SIZE

typedef struct StringBuilder {
    char *string;
    size_t length;
    size_t size;
} StringBuilder;

#define SB_PREP_ARRAY(sb_array, min_size)                                                                 \
do {                                                                                                      \
    if ((min_size) > (sb_array)->size) {                                                                  \
        if ((sb_array)->size == 0) {                                                                      \
            (sb_array)->size = SB_DEFAULT_SIZE;                                                           \
        }                                                                                                 \
        while ((min_size) > (sb_array)->size) {                                                           \
            (sb_array)->size *= 2;                                                                        \
        }                                                                                                 \
        (sb_array)->string = realloc((sb_array)->string, (sb_array)->size * sizeof(*(sb_array)->string)); \
        assert((sb_array)->string != NULL);                                                               \
    }                                                                                                     \
} while (0)                                                                                               \

#define SB_PUSH_CHAR(sb, c)              \
do {                                     \
    SB_PREP_ARRAY((sb), (sb)->length+1); \
    (sb)->string[(sb)->length++] = (c);  \
} while (0)                              \

#define SB_FREE(sb)      \
do {                     \
    free((sb)->string);  \
    (sb)->length = 0;    \
    (sb)->size = 0;      \
} while (0)              \

#define SB_CLEAR(sb)                     \
do {                                     \
    memset((sb)->string, 0, (sb)->size); \
    (sb)->length = 0;                    \
} while (0)                              \

#endif //SB_H