#ifndef _BG_ARENA_C
#define _BG_ARENA_C

#include <stddef.h>
#include <stdlib.h>

typedef unsigned int u32;
typedef int          i32;

typedef struct SimpleArena {
  void* buf;
  u32   size;
  u32   idx;
} SimpleArena;

enum ArenaErrorType {
  NO_ERR_ARENA_ERR_TYPE = 0,
  NULL_POINTER_ARENA_ERR_TYPE,
  SIZE_EXCEEDED_ARENA_ERR_TYPE,
};

SimpleArena* init_arena(const u32 size, i32* err) {
  const u32 struct_size = sizeof(SimpleArena);
  const u32 total_size  = struct_size + size;

  void* buf = malloc(total_size);
  if (!buf) {
    *err = NULL_POINTER_ARENA_ERR_TYPE;
    return 0;
  }
  SimpleArena* arena = (SimpleArena*)buf;
  arena->buf         = (void*)(buf + struct_size);
  arena->size        = size;
  arena->idx         = 0;
  return arena;
}

void free_arena(SimpleArena* arena) { free(arena); }

void* alloc_arena(SimpleArena* arena, const u32 size, i32* err) {
  if (!arena) {
    *err = NULL_POINTER_ARENA_ERR_TYPE;
    return 0;
  }
  // TODO : > or >= ?
  const u32 new_idx = arena->idx + size;
  if (new_idx > arena->size) {
    *err = SIZE_EXCEEDED_ARENA_ERR_TYPE;
    return 0;
  }
  arena->idx = new_idx;
  return (void*)(arena->buf + new_idx);
}

#endif  // _BG_ARENA_C
