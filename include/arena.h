#ifndef ARENA_H
#define ARENA_H

#include <stdalign.h>
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

#ifndef STRING_ARENA_DEFAULT_CAPACITY
#   define STRING_ARENA_DEFAULT_CAPACITY 32
#endif // STRING_ARENA_DEFAULT_CAPACITY

/* ----------------------------------------- SECTION: Arena ----------------------------------------- */

// This is to make type-safety more precise.
#define ArenaPtr char*

#ifndef ARENA_DEFAULT_CAPACITY
#   define ARENA_DEFAULT_CAPACITY 1024
#endif // ARENA_DEFAULT_CAPACITY

typedef struct ArenaRegion { 
    struct ArenaRegion *next;   // Points to the next ArenaRegion.
    ArenaPtr ptr;               // Points to the origin of allocated region.
    ArenaPtr use;               // Points to the used memory in this region.
    ArenaPtr top;               // Points to the top of the memory allocated in this region.
} ArenaRegion;

typedef struct Arena { 
    ArenaRegion *ptr;           // Points to the origin of allocated arena.
    ArenaRegion *use;           // Points to the used memory in this arena.
#ifdef TRACK_ARENA_ALLOCATION
    size_t allocs;              // How many regions were allocated
#endif // TRACK_ARENA_ALLOCATION
} Arena;

/*
    Vizualization of an Arena ( hyphotetically )

                 ┌REGION┐  ┌────REGION────┐
    ARENA      {[........][................]}
    ├ptr─────────┘                        │
    └use──────────────────────────────────┘

                ┌─Written─┐┌─Free─Bytes─┐
    REGION     [xxxxxxxxxxx..............]
    ├ptr────────┘         │             │ 
    ├use──────────────────┘             │ 
    └top────────────────────────────────┘
*/

/*
    Allocates a new `Arena` struct and returns a pointer to it, the arena
    can be then free'd with `arena_destroy` after use.
    NOTE: This invalidates all pointers allocated in the `Arena`.
*/
Arena *arena_init(Arena *arena);

// Not implemented because this would require putting `size` into the Arena struct
Arena *arena_sized_init(Arena *arena, size_t size);

// Perform pointer alignment.
void *arena_align(void *ptr, size_t align);

/*
    Allocates `ARENA_DEFAULT_CAPACITY ^ 2` bytes to this arena.
    Sets the base pointer to `arena->ptr` and `arena->use`.
    Sets the capacity `(arena->ptr + ARENA_DEFAULT_CAPACITY * 2)` to `arena->top`
*/
void *arena_alloc(Arena *arena, size_t bytes, size_t align);

/*
    Free the allocated memory in `arena.ptr` invalidating any pointer stored here and
    subsequentially setting all fields to NULL.
*/
void arena_destroy(Arena *arena);

// Print the `arena.ptr`, `arena.use` and `arena.top` addresses.
void arena_log(Arena arena);

/*
    NOTE: (25-12-26 18:51:28)
    This macro implies the allocation of a object but you provide the
    pointer to the arena and the type which the macro will expands to
    replace with `arena_alloc` call.
*/
#define arena_new(arena, type) (type*)arena_alloc(arena, sizeof(type), alignof(type));

/* ----------------------------------------- SECTION: StringArena ----------------------------------------- */

// Prepares for reallocation of the memory in case it's below the minimum size.
#define sa_prep(array, min_size)                                           \
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
    Allocates a new `StringArena` struct and returns a pointer to it.
    User owns the memory and must destroy the StringArena with sa_destroy().
*/
StringArena *sa_init();

// Free's the string and then free's the StringArena allocated by `sa_init()`.
void sa_destroy(StringArena *sa);

// Clears the content of StringArena to allow it to be reused.
void sa_clear(StringArena *sa);

// `memcpy()` the `raw_string` into StringArray and returns a pointer to that string.
char *sa_dup(StringArena *sa, char *string);

// Print the StringArray into console.
void sa_log(StringArena *sa);

#endif // ARENA_H