#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "haversine_formula.cpp"

typedef unsigned int u32;
typedef double f64;

extern f64 ReferenceHaversine(f64 X0, f64 Y0, f64 X1, f64 Y1, f64 EarthRadius);
extern const f64 REF_EARTH_RADIUS_KM;

static const char *json_filename = "haversine_input.json";
static const char *result_filename = "haversine_result.txt";
static const f64 haversine_x_upper = 180.0;
static const f64 haversine_x_lower = -180.0;
static const f64 haversine_y_upper = 90.0;
static const f64 haversine_y_lower = -90.0;

inline f64 gen_rand_float(const f64 upper, const f64 lower) {
  // initial result from [0.0 to 1.0]
  const f64 initial = (f64)rand() / (f64)RAND_MAX;
  // scale and shift
  const f64 result = (initial * fabs(upper - lower)) + lower;
  return result;
}

int gen_write_all(u32 random_seed, u32 num_pairs) {
  FILE *jsonfile = fopen(json_filename, "w");
  if (jsonfile == NULL) {
    fprintf(stderr, "Could not open file %s for writing\n", json_filename);
    return EXIT_FAILURE;
  }
  printf("Generating JSON output...\n");
  srand(random_seed);

  f64 sum = 0;
  fprintf(jsonfile, "{\n\t\"pairs\": [\n");
  for (u32 i = 0; i < num_pairs - 1; i++) {
    f64 x0 = gen_rand_float(haversine_x_upper, haversine_x_lower);
    f64 x1 = gen_rand_float(haversine_x_upper, haversine_x_lower);
    f64 y0 = gen_rand_float(haversine_y_upper, haversine_y_lower);
    f64 y1 = gen_rand_float(haversine_y_upper, haversine_y_lower);

    fprintf(jsonfile, "\t\t{\"x0\": %f, \"x1\": %f, \"y0\": %f, \"y1\": %f},\n",
            x0, x1, y0, y1);
    // TODO: potential overflow?
    sum += ReferenceHaversine(x0, y0, x1, y1, REF_EARTH_RADIUS_KM);
  }
  f64 last_x0 = gen_rand_float(haversine_x_upper, haversine_x_lower);
  f64 last_x1 = gen_rand_float(haversine_x_upper, haversine_x_lower);
  f64 last_y0 = gen_rand_float(haversine_y_upper, haversine_y_lower);
  f64 last_y1 = gen_rand_float(haversine_y_upper, haversine_y_lower);

  fprintf(jsonfile,
          "\t\t{\"x0\": %f, \"x1\": %f, \"y0\": %f, \"y1\": %f}\n\t]\n}\n",
          last_x0, last_x1, last_y0, last_y1);
  fclose(jsonfile);

  sum += ReferenceHaversine(last_x0, last_y0, last_x1, last_y1,
                            REF_EARTH_RADIUS_KM);
  f64 avg = sum / num_pairs;
  FILE *resultfile = fopen(result_filename, "w");
  if (resultfile == NULL) {
    fprintf(stderr, "Could not open file %s for writing\n", result_filename);
    return EXIT_FAILURE;
  }
  fprintf(resultfile, "%f", avg);
  fclose(resultfile);

  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s RANDOM_SEED NUM_PAIRS\n", argv[0]);
    return EXIT_FAILURE;
  } else {
    u32 random_seed = atoi(argv[1]);
    u32 num_pairs = atoi(argv[2]);
    if (num_pairs == 0) {
      fprintf(stderr, "NUM_PAIRS must be a positive integer (given %s)\n",
              argv[2]);
      return EXIT_FAILURE;
    }

    printf("Random seed: %10d\n", random_seed);
    printf("Num pairs  : %10d\n", num_pairs);
    const int result = gen_write_all(random_seed, num_pairs);
    if (result == EXIT_SUCCESS) {
      printf("Generated JSON output in file %s\n", json_filename);
    }
    return result;
  }
  return EXIT_SUCCESS;
}
