#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* ----------------------------------------- SECTION: Utilities ----------------------------------------- */ 

// Cast to void.
#define dead(t) (void)t

/* ----------------------------------------- SECTION: Array Generics ----------------------------------------- */
/*  
    NOTE: (25-12-06 21:08:59)
    Reference: https://github.com/tsoding/nob.h/
*/

#ifndef ARRAY_DEFAULT_SIZE
    #define ARRAY_DEFAULT_SIZE 256
#endif // ARRAY_DEFAULT_SIZE

// Performs reallocation of the memory in case it's below the minimum size.
#define array_init(array, min_size)                                                        \
do {                                                                                       \
    if ((min_size) > (array)->size) {                                                      \
        if ((array)->size == 0) {                                                          \
            (array)->size = ARRAY_DEFAULT_SIZE;                                            \
        }                                                                                  \
        while ((min_size) > (array)->size) {                                               \
            (array)->size *= 2;                                                            \
        }                                                                                  \
        (array)->items = realloc((array)->items, (array)->size * sizeof(*(array)->items)); \
        assert((array)->items != NULL);                                                    \
    }                                                                                      \
} while (0)

// Appends an element to the array.
#define array_append(array, elem)            \
do {                                         \
    array_init((array), (array)->len+1);     \
    (array)->items[(array)->len++] = (elem); \
} while (0)

// Extend the array with items
#define array_expand(array, nitems) array_expandn((array), (nitems), (nitems)->len)

// Extend the array with items by the length
#define array_expandn(array, nitems, nlength)                                                    \
do {                                                                                             \
    array_init((array), (array)->len + (nlength));                                               \
    memcpy((array)->items + (array)->len, (nitems)->items, (nlength) * sizeof(*(array)->items)); \
    (array)->len += (nlength);                                                                   \
} while (0)
    
// For primitive types mostly.(?)
#define array_len(array) (sizeof((array)) / sizeof(*(array)))

// Shift the arg and returns the value; Effectively popping an arg.
#define array_shift_arg(count, array) (assert((count) > 0 && "Length of given array is less or equal to zero."), (count)--, *(array)++)

// Shift the arg from the array and returns the value; Effectively popping an arg.
#define array_shift(array) (assert((array)->len > 0 && "Length of given array is less or equal to zero."), (array)->len--, *(array)->items++)

// Return the last arg from the array.
#define array_last(array) (assert((array)->len > 0 && "Length of given array is less or equal to zero."), (array)->items[(array)->len-1])

// Prints the basic data in the array.
#define array_log(array)                     \
do {                                         \
    printf("Pointer: %p\n", (array)->items); \
    printf("Length: %ld\n", (array)->len);   \
    printf("Size: %ld\n", (array)->size);    \
} while (0)

/*  
    NOTE: (25-12-06 23:40:15)
    > Stripping prefixes from certain macros to make it easier to use.
    > Might add collision to built-in names.(?)
*/
#ifdef ARRAY_STRIP_PREFIX
    #define shift_arg array_shift_arg
    #define shift array_shift
    #define len array_len

    #define append array_append
    #define expand array_expand

    #define last array_last
#endif // ARRAY_STRIP_PREFIX

/* ----------------------------------------- SECTION: StringArray ----------------------------------------- */

// Basic data-structure for array of strings.
typedef struct StringArray {
    char **items;
    size_t size;
    size_t len;
} StringArray;

// Prints every string in the array.
#define sb_log(array)                           \
do {                                            \
    for (size_t i = 0; i < (array)->len; ++i) { \
        printf("%s\n", (array)->items[i]);      \
    }                                           \
} while (0)

// Expand the array with items by the length
#define sb_expand(array, ...) sb_expandn((array), ((const char*[]){__VA_ARGS__}), sizeof(((const char*[]){__VA_ARGS__}))/sizeof(const char*))

// Expand the array with items by the length
#define sb_expandn(array, nitems, nlength)                                                \
do {                                                                                      \
    array_init((array), (array)->len + (nlength));                                        \
    memcpy((array)->items + (array)->len, (nitems), (nlength) * sizeof(*(array)->items)); \
    (array)->len += (nlength);                                                            \
} while (0)

/* ----------------------------------------- SECTION: StringBuilder ----------------------------------------- */

// Basic data-structure for string manipulation.
typedef struct StringBuilder {
    char *items;
    size_t size;
    size_t len;
} StringBuilder;

// Print the string in StringBuilder.
#define sb_print(array) printf("%s\n", (array)->items)

// Clears the StringBuilder and sets it's length to 0
#define sb_clear(array)                       \
do {                                          \
    memset((array)->items, 0, (array)->size); \
    (array)->len = 0;                         \
} while (0)

/* ----------------------------------------- SECTION: Aliases ----------------------------------------- */

#define sb_append array_append

#endif // ARRAY_H
