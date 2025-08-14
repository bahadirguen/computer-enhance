#include <stdio.h>

typedef unsigned int u32;
typedef int          i32;
typedef double       f64;

enum JsonValType {
  UNKNOWN_JSON_VAL_TYPE,
  FLOAT_JSON_VAL_TYPE,
};

typedef struct JsonObj {
  char*            key;
  void*            val;
  enum JsonValType type_val;
  struct JsonObj** children;
  u32              num_children;
} JsonObj;

/*
 * errors
 */
enum JsonErrorType {
  NO_ERR_JSON_ERR_TYPE,
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
JsonObj* json_get_ckey(JsonObj* json_obj, char* key, i32* json_err);
JsonObj* json_get_idx(JsonObj* json_obj, const u32 idx, i32* json_err) {
  if (json_obj == 0) {
    *json_err = NULL_POINTER_JSON_ERR_TYPE;
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
  if (json_obj == 0) {
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
  f64      my_float_val = 3.14f;
  JsonObj  valid_json_obj = {.val = (void*)(&my_float_val),
                             .type_val = FLOAT_JSON_VAL_TYPE};
  JsonObj  invalid_json_obj = {.val = (void*)(&my_float_val),
                               .type_val = UNKNOWN_JSON_VAL_TYPE};
  JsonObj* children[2] = {&valid_json_obj, &invalid_json_obj};
  JsonObj  full_json = {.children = children, .num_children = 2};
  f64      result = 0.0f;

  enum JsonErrorType err = NO_ERR_JSON_ERR_TYPE;

  JsonObj* valid_json_from_idx = json_get_idx(&full_json, 0, &err);
  JsonObj* invalid_json_from_idx = json_get_idx(&full_json, 1, &err);
  JsonObj* null_json_from_idx = json_get_idx(&full_json, 2, &err);

  result = json_to_float(&valid_json_obj, &err);
  printf("Valid JSON (obj) as float  : %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  result = json_to_float(valid_json_from_idx, &err);
  printf("Valid JSON (idx) as float  : %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  result = json_to_float(&invalid_json_obj, &err);
  printf("Invalid JSON (obj) as float: %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  result = json_to_float(invalid_json_from_idx, &err);
  printf("Invalid JSON (idx) as float: %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  result = json_to_float(0, &err);
  printf("Null JSON (obj) as float   : %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  result = json_to_float(null_json_from_idx, &err);
  printf("Null JSON (idx) as float   : %.3f (err %2d: %s)\n", result, err,
         json_err_to_cstr(err));
  return 0;
}
