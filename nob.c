#define _CRT_SECURE_NO_WARNINGS

#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER ""

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  if (!nob_mkdir_if_not_exists(BUILD_FOLDER))
    return 1;

  Nob_Cmd cmd = {0};

  nob_cmd_append(&cmd, "clang++");
  nob_cmd_append(&cmd, "-Wall", "-Wextra");
  nob_cmd_append(&cmd, "-o", BUILD_FOLDER "haversine_gen.exe");
  nob_cmd_append(&cmd, SRC_FOLDER "haversine_gen.cpp");

  if (!nob_cmd_run_sync_and_reset(&cmd))
    return 1;

  return 0;
}
