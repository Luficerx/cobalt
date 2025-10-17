#ifndef DA_H
#define DA_H

#define MIN_DA_SIZE 256
#define DA_PREP_ARRAY(array, min_size)                                                     \
do {                                                                                       \
    if ((min_size) > (array)->size) {                                                      \
        if ((array)->size == 0) (array)->size = MIN_DA_SIZE;                               \
        while ((min_size) > (array)->size) {                                               \
            (array)->size *= 2;                                                            \
        }                                                                                  \
        (array)->items = realloc((array)->items, (array)->size * sizeof(*(array)->items)); \
        assert((array)->items != NULL);                                                    \
    }                                                                                      \
} while (0)

#define DA_APPEND(da, it)               \
do {                                    \
    DA_PREP_ARRAY((da), (da)->count+1); \
    (da)->items[(da)->count++] = (it);  \
} while (0)

#endif // DA_H