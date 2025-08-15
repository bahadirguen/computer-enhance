#ifndef _BG_STRING_C
#define _BG_STRING_C

#include <string.h>

#include "arena.c"

typedef unsigned int u32;
typedef int          i32;

/*
 * allocates len+1 bytes for the buffer
 * last character of buf is '\0'
 */
typedef struct String {
  char* c_str;
  u32   len;
} String;

enum StringErrorType {
  NO_ERR_STRING_ERR_TYPE = 0,
  NULL_POINTER_STRING_ERR_TYPE,
  MEM_ALLOC_STRING_ERR_TYPE,
  MAX_LENGTH_EXCEEDED_STRING_ERR_TYPE,
};

const u32 MAX_LEN_C_STR = 1 << 15;

/*
 * Return 0 if lhs or rhs is NULL.
 * Return (lhs->len - rhs->len) if there is a length difference.
 * Return the result of strncpy() otherwise.
 */
i32 string_compare(String* lhs, String* rhs) {
  if (!lhs || !rhs) {
    return 0;
  }
  const i32 len_diff = lhs->len - rhs->len;

  if (!len_diff) {
    return strncmp(lhs->c_str, rhs->c_str, lhs->len);
  } else {
    return len_diff;
  }
}

String* string_from_c_str(char* c_str, SimpleArena* arena, i32* err) {
  if (!c_str) {
    *err = NULL_POINTER_STRING_ERR_TYPE;
    return 0;
  }

  u32 size_str = 0;
  for (u32 i = 0; i < MAX_LEN_C_STR; i++) {
    if (c_str[i] == '\0') {
      size_str = i + 1;
      break;
    }
  }
  if (!size_str) {
    *err = MAX_LENGTH_EXCEEDED_STRING_ERR_TYPE;
    return 0;
  }
  i32   arena_err = 0;
  void* buf       = alloc_arena(arena, sizeof(String) + size_str, &arena_err);
  if (arena_err) {
    *err = MEM_ALLOC_STRING_ERR_TYPE;
    return 0;
  }
  String* string = (String*)buf;
  string->c_str  = (char*)(buf + sizeof(String));
  string->len    = size_str - 1;
  memcpy(string->c_str, c_str, size_str);
  return string;
}

#endif  // _BG_STRING_C
