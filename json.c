#include <stdio.h>

#include "arena.c"
#include "string.c"

typedef unsigned int u32;
typedef int          i32;
typedef double       f64;

enum JsonValType {
  UNKNOWN_JSON_VAL_TYPE = 0,
  OBJ_JSON_VAL_TYPE     = 1 << 1,
  ARRAY_JSON_VAL_TYPE   = 1 << 2,
  FLOAT_JSON_VAL_TYPE,
};

typedef struct JsonObj {
  String*          key;
  void*            val;
  enum JsonValType type_val;
  u32              num_children;
  struct JsonObj** children;
} JsonObj;

/*
 * errors
 */
enum JsonErrorType {
  NO_ERR_JSON_ERR_TYPE = 0,
  NULL_POINTER_JSON_ERR_TYPE,
  INVALID_VAL_TYPE_JSON_ERR_TYPE,
  NON_EXISTING_INDEX_JSON_ERR_TYPE,
};

const char* json_err_to_cstr(const enum JsonErrorType json_err) {
  switch (json_err) {
    case NO_ERR_JSON_ERR_TYPE:
      return "no error";
    case NULL_POINTER_JSON_ERR_TYPE:
      return "null pointer";
    case INVALID_VAL_TYPE_JSON_ERR_TYPE:
      return "invalid value type";
    case NON_EXISTING_INDEX_JSON_ERR_TYPE:
      return "non-existing index";
    default:
      return "unknown error code";
  }
}

/*
 * parse
 */
JsonObj* cstr_to_json(char* json_str, i32* json_err);

/*
 * access
 */
JsonObj* json_get_key(JsonObj* json_obj, String* key, i32* json_err) {
  if (!json_obj) {
    *json_err = NULL_POINTER_JSON_ERR_TYPE;
    return 0;
  }
  if (json_obj->type_val != OBJ_JSON_VAL_TYPE) {
    *json_err = INVALID_VAL_TYPE_JSON_ERR_TYPE;
    return 0;
  }
  JsonObj* child = 0;
  for (u32 i = 0; i < json_obj->num_children; i++) {
    if (0 == string_compare(key, json_obj->children[i]->key)) {
      child = json_obj->children[i];
      break;
    }
  }
  return child;
}

JsonObj* json_get_idx(JsonObj* json_obj, const u32 idx, i32* json_err) {
  if (!json_obj) {
    *json_err = NULL_POINTER_JSON_ERR_TYPE;
    return 0;
  }
  if ((json_obj->type_val & (OBJ_JSON_VAL_TYPE | ARRAY_JSON_VAL_TYPE)) == 0) {
    *json_err = INVALID_VAL_TYPE_JSON_ERR_TYPE;
    return 0;
  }
  if (json_obj->num_children <= idx) {
    *json_err = NON_EXISTING_INDEX_JSON_ERR_TYPE;
    return 0;
  }
  *json_err = NO_ERR_JSON_ERR_TYPE;
  return json_obj->children[idx];
}

/*
 * convert
 */
f64 json_to_float(JsonObj* json_obj, i32* json_err) {
  if (!json_obj) {
    *json_err = NULL_POINTER_JSON_ERR_TYPE;
    return 0;
  }
  if (json_obj->type_val != FLOAT_JSON_VAL_TYPE) {
    *json_err = INVALID_VAL_TYPE_JSON_ERR_TYPE;
    return 0;
  }
  *json_err = NO_ERR_JSON_ERR_TYPE;
  return *(f64*)(json_obj->val);
}

int main() {
  f64      my_float_val_1     = 1.1f;
  f64      my_float_val_2     = 2.2f;
  JsonObj  valid_json_obj_1   = {.val      = (void*)(&my_float_val_1),
                                 .type_val = FLOAT_JSON_VAL_TYPE};
  JsonObj  invalid_json_obj_1 = {.val      = (void*)(&my_float_val_2),
                                 .type_val = UNKNOWN_JSON_VAL_TYPE};
  JsonObj  null_json_obj      = {0};
  JsonObj* children[2]        = {&valid_json_obj_1, &invalid_json_obj_1};
  JsonObj  full_json          = {.children = children, .num_children = 2};
  f64      result             = 0.0f;

  enum JsonErrorType err = NO_ERR_JSON_ERR_TYPE;

  JsonObj* invalid_json_obj_2 = json_get_idx(&full_json, 0, &err);
  full_json.type_val          = ARRAY_JSON_VAL_TYPE;
  JsonObj* valid_json_obj_2   = json_get_idx(&full_json, 0, &err);

  result = json_to_float(&null_json_obj, &err);
  printf("Null JSON as float       : %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  result = json_to_float(&invalid_json_obj_1, &err);
  printf("Invalid JSON (1) as float: %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  result = json_to_float(invalid_json_obj_2, &err);
  printf("Invalid JSON (2) as float: %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  result = json_to_float(&valid_json_obj_1, &err);
  printf("Valid JSON (1) as float  : %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  result = json_to_float(valid_json_obj_2, &err);
  printf("Valid JSON (2) as float  : %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));

  err                  = 0;
  SimpleArena* arena   = init_arena(4096, &err);
  String*      key_1   = string_from_c_str("k1", arena, &err);
  String*      key_2   = string_from_c_str("k2", arena, &err);
  valid_json_obj_1.key = key_1;
  full_json.type_val   = OBJ_JSON_VAL_TYPE;

  printf("Added keys %s and %s\n", key_1->c_str, key_2->c_str);

  JsonObj* json_obj_via_key_1 = json_get_key(&full_json, key_1, &err);
  if (err) {
    printf("JSON key access error (err %2d: %s)\n", err, json_err_to_cstr(err));
    err = 0;
  }
  result = json_to_float(json_obj_via_key_1, &err);
  printf("JSON via key (%s) as float  : %.3f (err %2d: %s)\n", key_1->c_str,
         result, err, json_err_to_cstr(err));
  JsonObj* json_obj_via_key_2 = json_get_key(&full_json, key_2, &err);
  if (err) {
    printf("JSON key access error (err %2d: %s)\n", err, json_err_to_cstr(err));
    err = 0;
  }
  result = json_to_float(json_obj_via_key_2, &err);
  printf("JSON via key (%s) as float  : %.3f (err %2d: %s)\n", key_2->c_str,
         result, err, json_err_to_cstr(err));

  free_arena(arena);
}
