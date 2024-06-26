//
// Created by hariharan on 8/8/22.
//

#include <dftracer/dftracer.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void foo() {
  DFTRACER_C_FUNCTION_START();
  DFTRACER_C_FUNCTION_UPDATE_INT("key", 0);
  DFTRACER_C_FUNCTION_UPDATE_STR("key", "0");
  sleep(1);
  DFTRACER_C_REGION_START(CUSTOM);
  DFTRACER_C_REGION_UPDATE_INT(CUSTOM, "key", 0);
  DFTRACER_C_REGION_UPDATE_STR(CUSTOM, "key", "0");
  sleep(1);
  DFTRACER_C_REGION_START(CUSTOM_BLOCK);
  sleep(1);
  DFTRACER_C_REGION_END(CUSTOM_BLOCK);
  DFTRACER_C_REGION_END(CUSTOM);
  DFTRACER_C_FUNCTION_END();
}

int main(int argc, char *argv[]) {
  int init = 0;
  if (argc > 2) {
    if (strcmp(argv[2], "1") == 0) {
      DFTRACER_C_INIT(NULL, NULL, NULL);
      init = 1;
    }
  }
  char filename[1024];
  sprintf(filename, "%s/demofile_c.txt", argv[1]);
  foo();
  FILE *fh = fopen(filename, "w+");
  fwrite("hello", sizeof("hello"), 1, fh);
  fclose(fh);
  if (init) {
    DFTRACER_C_FINI();
  }
  return 0;
}