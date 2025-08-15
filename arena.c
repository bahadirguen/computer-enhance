#ifndef _BG_ARENA_C
#define _BG_ARENA_C

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int u32;
typedef int          i32;
typedef double       f64;

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

/*
 * stack
 */

enum StackDataType {
  I32_STACK_DATA_TYPE,
  U32_STACK_DATA_TYPE,
  F64_STACK_DATA_TYPE,
};

u32 get_stack_data_type_size(enum StackDataType data_type) {
  switch (data_type) {
    case I32_STACK_DATA_TYPE:
      return sizeof(i32);
    case U32_STACK_DATA_TYPE:
      return sizeof(u32);
    case F64_STACK_DATA_TYPE:
      return sizeof(f64);
    default:
      return 0;
  }
}

union StackDataValue {
  i32 val_i32;
  u32 val_u32;
  f64 val_f64;
};

typedef struct StackData {
  enum StackDataType   data_type;
  union StackDataValue value;
} StackData;

typedef struct IndexLink {
  u32                idx;
  enum StackDataType prev_data_type;
  struct IndexLink*  prev;
} IndexLink;

/*
 * Only supports specified data types.
 * Uses additional memory for one IndexLink per push.
 */
typedef struct SimpleStack {
  void*      buf;
  u32        size;
  IndexLink* link;
} SimpleStack;

enum StackErrorType {
  NO_ERR_STACK_ERR_TYPE = 0,
  NULL_POINTER_STACK_ERR_TYPE,
  SIZE_EXCEEDED_STACK_ERR_TYPE,
  UNKNOWN_TYPE_STACK_ERR_TYPE,
};

SimpleStack* init_stack(const u32 size, i32* err) {
  const u32 stack_struct_size = sizeof(SimpleStack);
  const u32 link_struct_size  = sizeof(IndexLink);
  const u32 total_size        = stack_struct_size + link_struct_size + size;

  void* buf = malloc(total_size);
  if (!buf) {
    *err = NULL_POINTER_STACK_ERR_TYPE;
    return 0;
  }
  SimpleStack* stack = (SimpleStack*)buf;
  stack->link        = (IndexLink*)(buf + stack_struct_size);
  stack->buf         = (void*)(buf + stack_struct_size + link_struct_size);
  stack->size        = size;
  *(stack->link)     = (IndexLink){0};
  return stack;
}

void free_stack(SimpleStack* stack) { free(stack); }

/*
 * Return error code.
 */
i32 push_stack(SimpleStack* stack, void* src, enum StackDataType data_type) {
  u32 data_size = get_stack_data_type_size(data_type);
  if (!data_size) {
    return UNKNOWN_TYPE_STACK_ERR_TYPE;
  }
  const u32 new_idx = stack->link->idx + data_size + sizeof(IndexLink);
  // TODO : > or >= ?
  if (new_idx > stack->size) {
    return SIZE_EXCEEDED_STACK_ERR_TYPE;
  }
  memcpy(&(stack->buf[stack->link->idx]), src, data_size);
  IndexLink* new_link = &(stack->buf[stack->link->idx + data_size]);
  *new_link           = (IndexLink){
                .idx = new_idx, .prev_data_type = data_type, .prev = stack->link};
  stack->link = new_link;
  return 0;
}

StackData pop_stack(SimpleStack* stack) {
  if (!stack || !(stack->link) || !(stack->link->prev)) {
    return (StackData){0};
  }
  IndexLink* prev       = stack->link->prev;
  void*      prev_buf   = (void*)&(stack->buf[prev->idx]);
  StackData  stack_data = {.data_type = stack->link->prev_data_type};

  union StackDataValue data_val = {0};
  switch (stack_data.data_type) {
    case I32_STACK_DATA_TYPE:
      data_val.val_i32 = *(i32*)prev_buf;
      break;
    case U32_STACK_DATA_TYPE:
      data_val.val_u32 = *(u32*)prev_buf;
      break;
    case F64_STACK_DATA_TYPE:
      data_val.val_f64 = *(f64*)prev_buf;
      break;
    default:
      break;
  }
  stack_data.value = data_val;
  stack->link      = prev;
  return stack_data;
}

#endif  // _BG_ARENA_C
