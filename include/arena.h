#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>
#include <assert.h>

/* ----------------------------------------- SECTION: AVAILABLE FLAGS ----------------------------------------- */
/*
    [TRACK_ARENA_ALLOCATION]
    - Enable `allocs` field that represents the amount of allocations done in all arenas.
    - undefined

    [STRING_ARENA_DEFAULT_CAPACITY]
    - Default value in bytes to allocate for StringArena.
    - 32
*/

#ifdef TRACK_ARENA_ALLOCATION
#   define __ARENA_ALLOCATED(array) (array)->allocs++
#else 
#   define __ARENA_ALLOCATED(array)
#endif // TRACK_ARENA_ALLOCATION

/* ----------------------------------------- SECTION: StringArena ----------------------------------------- */

#ifndef STRING_ARENA_DEFAULT_CAPACITY
#   define STRING_ARENA_DEFAULT_CAPACITY 32
#endif // STRING_ARENA_DEFAULT_CAPACITY

// Performs reallocation of the memory in case it's below the minimum size.
#define string_arena_init(array, min_size)                                 \
do {                                                                       \
    if ((min_size) > (array)->size) {                                      \
        __ARENA_ALLOCATED((array));                                        \
        if ((array)->size == 0) {                                          \
            (array)->size = STRING_ARENA_DEFAULT_CAPACITY;                 \
        }                                                                  \
        while ((min_size) > (array)->size) { (array)->size *= 2; }         \
        (array)->items = realloc((array)->items,                           \
                                 (array)->size * sizeof(*(array)->items)); \
        assert((array)->items != NULL);                                    \
    }                                                                      \
} while (0)

typedef struct StringArena {
    char *items;   // String content
    size_t size;   // Bytes allocated 
    size_t used;   // How many bytes were actually used
    size_t len;    // The amount of strings that were appended

#ifdef TRACK_ARENA_ALLOCATION
    size_t allocs; // How many reallocs were performed in this StringArena
#endif // TRACK_ARENA_ALLOCATION

} StringArena;

/*
    Allocates a new StringArena object and returns a pointer to it.
    User owns the memory and must destroy the StringArena with sa_destroy().
*/
StringArena *sa_init();

// Free's the string and then free's the StringArena allocated by `sa_init()`.
void sa_destroy(StringArena *sa);

// Clears the content of StringArena to allow it to be reused.
void sa_clear(StringArena *sa);

// `memcpy()` the `raw_string` into StringArray and returns a pointer to that string.
char *sa_dup(StringArena *sa, char *string);

// Logs the StringArray into console.
void sa_log(StringArena *sa);

#endif // ARENA_H