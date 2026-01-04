#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "arena.h"
#include "core.h"

Arena *arena_init(Arena *arena) {
    size_t size = (ARENA_DEFAULT_CAPACITY * ARENA_DEFAULT_CAPACITY);
    
    ArenaRegion *region_ptr = malloc(sizeof(*region_ptr));
    if (region_ptr == NULL) THROW("Could not malloc ArenaRegion.");
    
    region_ptr->ptr = malloc(size);
    if (region_ptr->ptr == NULL) THROW("Could not malloc ArenaRegion.");
   
    region_ptr->use = region_ptr->ptr;
    region_ptr->top = region_ptr->ptr + size;
    region_ptr->next = NULL;
    
    arena->ptr = region_ptr;
    arena->use = region_ptr;
    arena->allocs = 1;
    return arena;
}

Arena *arena_sized_init(Arena *arena, size_t size) {
    DEAD(arena);
    DEAD(size);
    NOIMPL(); 
}

void *arena_align(void *ptr, size_t align) {
    uintptr_t iptr = (uintptr_t)ptr;
    size_t rest = iptr % align;
    if (rest != 0) { iptr += align - rest; }
    return (void*)iptr;
}

void *arena_alloc(Arena *arena, size_t bytes, size_t align) {
    ArenaRegion *region = arena->use;
    void *ptr = arena_align(region->use, align);

    if ((ArenaPtr)ptr + bytes > region->top) {
        size_t region_size = (ARENA_DEFAULT_CAPACITY * ARENA_DEFAULT_CAPACITY);
        
        if (region_size < 2) THROW("ARENA_DEFAULT_CAPACITY must be >= 2.");

        ArenaRegion *new_region = malloc(sizeof(*new_region));
        new_region->ptr = malloc(region_size);
        new_region->use = new_region->ptr;
        new_region->top = new_region->ptr + region_size;
        new_region->next = NULL;

        region->next = new_region;
        arena->use = new_region;
        ptr = new_region->ptr;
        region = new_region;
        arena->allocs++;
    }

    region->use = (ArenaPtr)ptr + bytes;
    return ptr;
}

void arena_destroy(Arena *arena) {
    ArenaRegion *region = arena->ptr;
    while (region) {
        ArenaRegion *next_ptr = region->next;
        free(region->ptr);
        free(region);
        region = next_ptr;
    }
}

void arena_log(Arena arena) { DEAD(arena); NOIMPL(); }

StringArena *sa_init() {
    StringArena *sa = malloc(sizeof(*sa));
    sa->items = NULL;
    sa->size = 0;
    sa->used = 0;
    sa->len = 0;
    
#ifdef TRACK_ARENA_ALLOCATION
    sa->allocs = 0;
#endif // TRACK_ARENA_ALLOCATION
    
    return sa;
}

void sa_destroy(StringArena *sa) {
    free(sa->items);
    free(sa);
}

void sa_clear(StringArena *sa) {
    memset(sa->items, 0, sa->size);
    sa->size = 0;
    sa->used = 0;
    sa->len = 0;

#ifdef TRACK_ARENA_ALLOCATION
    sa->allocs = 0;
#endif // TRACK_ARENA_ALLOCATION
}

char *sa_dup(StringArena *sa, char *raw_string) {
    size_t len = strlen(raw_string) + 1; // +1 Include null terminator
    sa_prep(sa, sa->used + len);
    
    char *string = sa->items + sa->used; // Points at the end of used memory
    memcpy(string, raw_string, len);
    
    sa->used += len;
    sa->len++;                           // Increment how many strings were added to this StringArray
    
    return string;
}

void sa_log(StringArena *sa) {
    size_t min = (sa->used < 10) ? sa->used : 10;
    for (size_t i = 0; i < min; ++i) {
        if (sa->items[i] == '\0') { printf("@"); continue; }
        printf("%c", sa->items[i]);
    }
    printf("\n");

    printf("items: \'%s\'\n", sa->items);
    printf("size: %ld\n", sa->size);
    printf("used: %ld\n", sa->used);

    size_t diff = (sa->size - sa->used);
    float diff_p = ((float)diff / (float)sa->size) * 100.0f;
    printf("diff: %ld | %.2f%%\n", diff, diff_p);
    printf("len: %ld\n", sa->len);
    
#ifdef TRACK_ARENA_ALLOCATION
    printf("allocs: %ld\n", sa->allocs);
#endif // TRACK_ARENA_ALLOCATION
}